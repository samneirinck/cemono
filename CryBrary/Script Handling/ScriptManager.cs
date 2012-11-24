using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Windows.Forms;
using System.Xml;
using System.Runtime.Serialization;
using System.Runtime.InteropServices;

using CryEngine.Async;
using CryEngine.Extensions;
using CryEngine.Native;
using CryEngine.Sandbox;
using CryEngine.Testing;
using CryEngine.Testing.Internals;

using CryEngine.Serialization;
using CryEngine.Utilities;

namespace CryEngine.Initialization
{
    class ScriptManager
    {
        public ScriptManager(bool initialLoad = true)
        {
            Instance = this;

            if (FlowNodes == null)
                FlowNodes = new List<string>();
            if (Scripts == null)
                Scripts = new List<CryScript>();

            if (!Directory.Exists(PathUtils.TempFolder))
                Directory.CreateDirectory(PathUtils.TempFolder);
            else
            {
                try
                {
                    foreach (var file in Directory.GetFiles(PathUtils.TempFolder))
                    {
                        if (!Path.HasExtension(".scriptdump"))
                            File.Delete(file);
                    }
                }
                catch(UnauthorizedAccessException) { }
            }

#if !UNIT_TESTING
            TestManager.Init();
#endif

            if (initialLoad)
                RegisterInternalTypes();

            Formatter = new CrySerializer();
        }

        public ScriptReloadResult Initialize(bool initialLoad)
        {
            var result = ScriptReloadResult.Success;

            try
            {
                LoadPlugins(initialLoad);
            }
            catch (Exception ex)
            {
                var scriptReloadMessage = new ScriptReloadMessage(ex, !initialLoad);
                scriptReloadMessage.ShowDialog();

                result = scriptReloadMessage.Result;
            }

            return result;
        }

        class ScriptManagerData : CryScriptInstance
        {
            public ScriptManagerData()
            {
                Input = new SerializableInput();
            }

            public SerializableInput Input { get; set; }
            public int LastScriptId { get; set; }
        }

        void Serialize()
        {
            var data = new ScriptManagerData();
            data.LastScriptId = LastScriptId;

            data.Input.ActionmapEvents = Input.ActionmapEvents;

            data.Input.KeyEvents = Input.KeyEventsInvocationList;
            data.Input.MouseEvents = Input.MouseEventsInvocationList;

            AddScriptInstance(data, ScriptType.CryScriptInstance);

            using (var stream = File.Create(SerializedScriptsFile))
                Formatter.Serialize(stream, Scripts);
        }

        void Deserialize()
        {
            using (var stream = File.Open(SerializedScriptsFile, FileMode.Open))
                Scripts = Formatter.Deserialize(stream) as List<CryScript>;

            File.Delete(SerializedScriptsFile);
            File.Delete(SerializedScriptManagerDataFile);

            var data = Find<ScriptManagerData>(ScriptType.CryScriptInstance, x => { return true; });

            LastScriptId = data.LastScriptId;

            Input.ActionmapEvents = data.Input.ActionmapEvents;

            if (data.Input.KeyEvents != null)
            {
                foreach (var keyDelegate in data.Input.KeyEvents)
                    Input.KeyEvents += keyDelegate as KeyEventDelegate;
            }

            if (data.Input.MouseEvents != null)
            {
                foreach (var mouseDelegate in data.Input.MouseEvents)
                    Input.MouseEvents += mouseDelegate as MouseEventDelegate;
            }

            RemoveInstance(data.ScriptId, ScriptType.CryScriptInstance);
        }

        /// <summary>
        /// Called from GameDll
        /// </summary>
        public void RegisterFlownodes()
        {
            // These have to be registered later on due to the flow system being initialized late.
            // Note: Flow nodes have to be registered from IGame::CompleteInit in order to be usable from within UI graphs. (Use IMonoScriptSystem::RegisterFlownodes)
            foreach (var node in FlowNodes)
                FlowNode.Register(node);

            FlowNodes.Clear();
        }

        public void OnRevert()
        {
            // Revert to previous state
        }

        void PopulateAssemblyLookup()
        {
#if !RELEASE
            // Doesn't exist when unit testing
            if (Directory.Exists(PathUtils.MonoFolder))
            {
                using (XmlWriter writer = XmlWriter.Create(Path.Combine(PathUtils.MonoFolder, "assemblylookup.xml")))
                {
                    writer.WriteStartDocument();
                    writer.WriteStartElement("AssemblyLookupTable");

                    var gacFolder = Path.Combine(PathUtils.MonoFolder, "lib", "mono", "gac");
                    foreach (var assemblyLocation in Directory.GetFiles(gacFolder, "*.dll", SearchOption.AllDirectories))
                    {
                        var separator = new [] { "__" };
                        var splitParentDir = Directory.GetParent(assemblyLocation).Name.Split(separator, StringSplitOptions.RemoveEmptyEntries);

                        var assembly = Assembly.Load(Path.GetFileName(assemblyLocation) + string.Format(", Version={0}, Culture=neutral, PublicKeyToken={1}", splitParentDir.ElementAt(0), splitParentDir.ElementAt(1)));

                        writer.WriteStartElement("Assembly");
                        writer.WriteAttributeString("name", assembly.FullName);

                        foreach (var nameSpace in assembly.GetTypes().Select(t => t.Namespace).Distinct())
                        {
                            writer.WriteStartElement("Namespace");
                            writer.WriteAttributeString("name", nameSpace);
                            writer.WriteEndElement();
                        }

                        writer.WriteEndElement();
                    }

                    writer.WriteEndElement();
                    writer.WriteEndDocument();
                }
            }
#endif
        }

        void RegisterInternalTypes()
        {
            CryScript script;
            if (CryScript.TryCreate(typeof(NativeActor), out script))
                Scripts.Add(script);

            if (CryScript.TryCreate(typeof(NativeEntity), out script))
            {
                var entityRegistrationParams = new EntityRegistrationParams();

                entityRegistrationParams.name = script.ScriptName;
                entityRegistrationParams.flags = EntityClassFlags.Default | EntityClassFlags.Invisible; 

                NativeEntityMethods.RegisterEntityClass(entityRegistrationParams);
                Scripts.Add(script);
            }
        }

        void LoadPlugins(bool initialLoad)
        {
            var pluginsDirectory = PathUtils.PluginsFolder;
            if (!Directory.Exists(pluginsDirectory))
                return;

            bool hasDefaultGameRules = false;
            foreach (var directory in Directory.GetDirectories(pluginsDirectory))
            {
                var compilerDll = Path.Combine(directory, "Compiler.dll");
                if (File.Exists(compilerDll))
                {
                    var assembly = LoadAssembly(compilerDll);

                    var compilerType = assembly.GetTypes().First(x => x.Implements<ScriptCompiler>());
                    Debug.LogAlways("        Initializing {0}...", compilerType.Name);

                    var compiler = Activator.CreateInstance(compilerType) as ScriptCompiler;

                    var assemblyPaths = Directory.GetFiles(directory, "*.dll", SearchOption.AllDirectories);
                    var assemblies = new List<Assembly>();

                    foreach (var assemblyPath in assemblyPaths)
                    {
                        if (assemblyPath != compilerDll)
                            assemblies.Add(LoadAssembly(assemblyPath));
                    }

                    var scripts = compiler.Process(assemblies);

                    foreach (var unprocessedScript in scripts)
                    {
                        var script = unprocessedScript;

                        if (initialLoad)
                        {
                            if (script.RegistrationParams == null)
                                continue;
                            else if (script.RegistrationParams is ActorRegistrationParams)
                            {
                                var registrationParams = (ActorRegistrationParams)script.RegistrationParams;

                                NativeActorMethods.RegisterActorClass(script.ScriptName, script.Type.Implements(typeof(NativeActor)));
                            }
                            else if (script.RegistrationParams is EntityRegistrationParams)
                            {
                                var registrationParams = (EntityRegistrationParams)script.RegistrationParams;

                                if (registrationParams.name == null)
                                    registrationParams.name = script.ScriptName;
                                if (registrationParams.category == null)
                                    registrationParams.category = "Default";

                                NativeEntityMethods.RegisterEntityClass(registrationParams);
                            }
                            else if (script.RegistrationParams is GameRulesRegistrationParams)
                            {
                                var registrationParams = (GameRulesRegistrationParams)script.RegistrationParams;

                                if (registrationParams.name == null)
                                    registrationParams.name = script.ScriptName;

                                NativeGameRulesMethods.RegisterGameMode(registrationParams.name);

                                if (registrationParams.defaultGamemode || !hasDefaultGameRules)
                                {
                                    NativeGameRulesMethods.SetDefaultGameMode(registrationParams.name);

                                    hasDefaultGameRules = true;
                                }
                            }
                            else if (script.RegistrationParams is FlowNodeRegistrationParams)
                            {
                                var registrationParams = (FlowNodeRegistrationParams)script.RegistrationParams;

                                if (registrationParams.name == null)
                                    registrationParams.name = script.ScriptName;
                                if (registrationParams.category == null)
                                    registrationParams.category = script.Type.Namespace;

                                script.ScriptName = registrationParams.category + ":" + registrationParams.name;

                                FlowNodes.Add(script.ScriptName);
                            }
                        }

                        Scripts.Add(script);
                    }
                }
            }
        }

        /// <summary>
        /// Loads a C# assembly by location, creates a shadow-copy and generates debug database (mdb).
        /// </summary>
        /// <param name="assemblyPath"></param>
        public Assembly LoadAssembly(string assemblyPath)
        {
#if !(RELEASE && RELEASE_DISABLE_CHECKS)
            if (assemblyPath == null)
                throw new ArgumentNullException("assemblyPath");
            if (assemblyPath.Length < 1)
                throw new ArgumentException("string cannot be empty!", "assemblyPath");
#endif

            var newPath = Path.Combine(PathUtils.TempFolder, Path.GetFileName(assemblyPath));

            TryCopyFile(assemblyPath, ref newPath);

#if !RELEASE
            GenerateDebugDatabaseForAssembly(assemblyPath);

            var mdbFile = assemblyPath + ".mdb";
            if (File.Exists(mdbFile)) // success
            {
                var newMdbPath = Path.Combine(PathUtils.TempFolder, Path.GetFileName(mdbFile));
                TryCopyFile(mdbFile, ref newMdbPath);
            }
#endif

            return Assembly.LoadFrom(newPath);
        }

        void TryCopyFile(string currentPath, ref string newPath, bool overwrite = true)
        {
            if (!File.Exists(newPath))
                File.Copy(currentPath, newPath, overwrite);
            else
            {
                try
                {
                    File.Copy(currentPath, newPath, overwrite);
                }
                catch(Exception ex)
                {
                    if (ex is UnauthorizedAccessException || ex is IOException)
                    {
                        newPath = Path.ChangeExtension(newPath, "_" + Path.GetExtension(newPath));
                        TryCopyFile(currentPath, ref newPath);
                    }
                    else
                        throw;
                }
            }
        }

        public void GenerateDebugDatabaseForAssembly(string assemblyPath)
        {
#if !(RELEASE && RELEASE_DISABLE_CHECKS)
            if (assemblyPath == null)
                throw new ArgumentNullException("assemblyPath");
            if (assemblyPath.Length < 1)
                throw new ArgumentException("string cannot be empty!", "assemblyPath");
#endif

            if (File.Exists(Path.ChangeExtension(assemblyPath, "pdb")))
            {
                var assembly = Assembly.LoadFrom(Path.Combine(PathUtils.MonoFolder, "bin", "pdb2mdb.dll"));
                var driver = assembly.GetType("Driver");
                var convertMethod = driver.GetMethod("Convert", BindingFlags.Static | BindingFlags.Public);

                object[] args = { assemblyPath };
                convertMethod.Invoke(null, args);
            }
        }

        /// <summary>
        /// Called once per frame.
        /// </summary>
        public void OnUpdate(float frameTime, float frameStartTime, float asyncTime, float frameRate, float timeScale)
        {
            Time.Set(frameTime, frameStartTime, asyncTime, frameRate, timeScale);

            Awaiter.Instance.OnUpdate(frameTime);

            Scripts.ForEach(x =>
            {
                if (x.ScriptType.ContainsFlag(ScriptType.CryScriptInstance) && x.ScriptInstances != null)
                {
                    x.ScriptInstances.ForEach(instance =>
                    {
                        if (instance.ReceiveUpdates)
                            instance.OnUpdate();
                    });
                }
            });
        }

        /// <summary>
        /// Instantiates a script using its name and interface.
        /// </summary>
        /// <param name="scriptName"></param>
        /// <param name="scriptType"></param>
        /// <param name="constructorParams"></param>
        /// <returns>New instance scriptId or -1 if instantiation failed.</returns>
        public CryScriptInstance CreateScriptInstance(string scriptName, ScriptType scriptType, object[] constructorParams = null, bool throwOnFail = true)
        {
#if !(RELEASE && RELEASE_DISABLE_CHECKS)
            if (scriptName == null)
                throw new ArgumentNullException("scriptName");
            if (scriptName.Length < 1)
                throw new ArgumentException("string cannot be empty!", "scriptName");
            if (!Enum.IsDefined(typeof(ScriptType), scriptType))
                throw new ArgumentException(string.Format("scriptType: value {0} was not defined in the enum", scriptType));
#endif

            var script = Scripts.FirstOrDefault(x => x.ScriptType.ContainsFlag(scriptType) && x.ScriptName.Equals(scriptName));
            if (script == default(CryScript))
            {
                if (throwOnFail)
                    throw new ScriptNotFoundException(string.Format("Script {0} of ScriptType {1} could not be found.", scriptName, scriptType));
                else
                    return null;
            }

            return CreateScriptInstance(script, constructorParams);
        }

        public CryScriptInstance CreateScriptInstance(CryScript script, object[] constructorParams = null)
        {
#if !(RELEASE && RELEASE_DISABLE_CHECKS)
            if (script == default(CryScript))
                throw new ArgumentNullException("script");
#endif

            var scriptInstance = Activator.CreateInstance(script.Type, constructorParams) as CryScriptInstance;
#if !(RELEASE && RELEASE_DISABLE_CHECKS)
            if (scriptInstance == null)
                throw new ArgumentException("Failed to create instance, make sure type derives from CryScriptInstance", "scriptName");
#endif

            if (script.ScriptType == ScriptType.GameRules)
                (scriptInstance as GameRules).InternalInitialize();

            AddScriptInstance(script, scriptInstance);

            return scriptInstance;
        }

        public void AddScriptInstance(CryScriptInstance instance, ScriptType scriptType)
        {
#if !(RELEASE && RELEASE_DISABLE_CHECKS)
            if (instance == null)
                throw new ArgumentNullException("instance");
            if (!Enum.IsDefined(typeof(ScriptType), scriptType))
                throw new ArgumentException(string.Format("scriptType: value {0} was not defined in the enum", scriptType));
#endif

            var script = FindScript(scriptType, x => x.Type == instance.GetType());
            if (script == default(CryScript))
            {
                if (CryScript.TryCreate(instance.GetType(), out script))
                    Scripts.Add(script);
                else
                    return;
            }

            AddScriptInstance(script, instance);
        }

        void AddScriptInstance(CryScript script, CryScriptInstance instance, int scriptId = -1)
        {
#if !(RELEASE && RELEASE_DISABLE_CHECKS)
            if (script == default(CryScript))
                throw new ArgumentException("script");
#endif

            var index = Scripts.IndexOf(script);

#if !(RELEASE && RELEASE_DISABLE_CHECKS)
            if (index == -1)
                throw new ArgumentException("Provided CryScript object was not present in the script collection", "script");
#endif

            instance.ScriptId = (scriptId != -1) ? scriptId : LastScriptId++;

            if (script.ScriptInstances == null)
                script.ScriptInstances = new List<CryScriptInstance>();

            script.ScriptInstances.Add(instance);

            Scripts[index] = script;
        }

        public void ReplaceScriptInstance(CryScriptInstance newInstance, int scriptId, ScriptType scriptType)
        {
            RemoveInstance(scriptId, scriptType);

            var script = FindScript(scriptType, x => x.Type == newInstance.GetType());
            if (script == default(CryScript))
            {
                if (CryScript.TryCreate(newInstance.GetType(), out script))
                    Scripts.Add(script);
                else
                    return;
            }

            AddScriptInstance(script, newInstance, scriptId);
        }

        public void RemoveInstance(int instanceId, ScriptType scriptType)
        {
            RemoveInstances<CryScriptInstance>(scriptType, x => x.ScriptId == instanceId);
        }

        /// <summary>
        /// Locates and removes the script with the assigned scriptId.
        /// </summary>
        public int RemoveInstances<T>(ScriptType scriptType, Predicate<T> match) where T : CryScriptInstance
        {
#if !(RELEASE && RELEASE_DISABLE_CHECKS)
            if (!Enum.IsDefined(typeof(ScriptType), scriptType))
                throw new ArgumentException(string.Format("scriptType: value {0} was not defined in the enum", scriptType));
#endif

            int numRemoved = 0;
            for (int i = 0; i < Scripts.Count; i++)
            {
                var script = Scripts[i];

                if (script.ScriptType.ContainsFlag(scriptType))
                {
                    if (script.ScriptInstances != null)
                    {
                        numRemoved += script.ScriptInstances.RemoveAll(x =>
                            {
                                if (match(x as T))
                                {
                                    x.OnDestroyedInternal();
                                    return true;
                                }

                                return false;
                            });
                    }
                }

                Scripts[i] = script;
            }

            return numRemoved;
        }

        public int RemoveInstances(ScriptType scriptType, Predicate<CryScriptInstance> match)
        {
            return RemoveInstances<CryScriptInstance>(scriptType, match);
        }

        public CryScriptInstance GetScriptInstanceById(int id, ScriptType scriptType)
        {
#if !(RELEASE && RELEASE_DISABLE_CHECKS)
            if (id == 0)
                throw new ArgumentException("instance id cannot be 0!");
#endif

            return Find<CryScriptInstance>(scriptType, x => x.ScriptId == id);
        }

        #region Linq statements
        public CryScript FindScript(ScriptType scriptType, Func<CryScript, bool> predicate)
        {
#if !(RELEASE && RELEASE_DISABLE_CHECKS)
            if (!Enum.IsDefined(typeof(ScriptType), scriptType))
                throw new ArgumentException(string.Format("scriptType: value {0} was not defined in the enum", scriptType));
#endif

            return Scripts.FirstOrDefault(x => x.ScriptType.ContainsFlag(scriptType) && predicate(x));
        }

        public void ForEachScript(ScriptType scriptType, Action<CryScript> action)
        {
#if !(RELEASE && RELEASE_DISABLE_CHECKS)
            if (!Enum.IsDefined(typeof(ScriptType), scriptType))
                throw new ArgumentException(string.Format("scriptType: value {0} was not defined in the enum", scriptType));
#endif

            Scripts.ForEach(x =>
            {
                if (x.ScriptType.ContainsFlag(scriptType))
                    action(x);
            });
        }

        public void ForEach(ScriptType scriptType, Action<CryScriptInstance> action)
        {
#if !(RELEASE && RELEASE_DISABLE_CHECKS)
            if (!Enum.IsDefined(typeof(ScriptType), scriptType))
                throw new ArgumentException(string.Format("scriptType: value {0} was not defined in the enum", scriptType));
#endif

            ForEachScript(scriptType, script =>
            {
                if (script.ScriptInstances != null)
                    script.ScriptInstances.ForEach(action);
            });
        }

        public T Find<T>(ScriptType scriptType, Func<T, bool> predicate) where T : CryScriptInstance
        {
#if !(RELEASE && RELEASE_DISABLE_CHECKS)
            if (!Enum.IsDefined(typeof(ScriptType), scriptType))
                throw new ArgumentException(string.Format("scriptType: value {0} was not defined in the enum", scriptType));
#endif

            T scriptInstance = null;

            ForEachScript(scriptType, script =>
            {
                if (script.ScriptInstances != null && script.Type.ImplementsOrEquals<T>())
                {
                    var instance = script.ScriptInstances.Find(x => !x.IsDestroyed && predicate(x as T)) as T;
                    if (instance != null)
                    {
                        scriptInstance = instance;
                        return;
                    }
                }
            });

            return scriptInstance;
        }
        #endregion

        /// <summary>
        /// Last assigned ScriptId, next = + 1
        /// </summary>
        public int LastScriptId = 1;

        public bool IgnoreExternalCalls { get; set; }

        internal List<CryScript> Scripts { get; set; }

        AppDomain ScriptDomain { get; set; }
        IFormatter Formatter { get; set; }

        List<string> FlowNodes { get; set; }

        string SerializedScriptsFile { get { return Path.Combine(PathUtils.TempFolder, "CompiledScripts.scriptdump"); } }
        string SerializedScriptManagerDataFile { get { return Path.Combine(PathUtils.TempFolder, "ScriptManagerData.scriptdump"); } }

        public static ScriptManager Instance;
    }

    /// <summary>
    /// Thrown by <see cref="CryEngine.Initialization.ScriptManager"/> if a CryScript could not be located.
    /// </summary>
    [Serializable]
    public class ScriptNotFoundException : Exception
    {
        public ScriptNotFoundException(string error)
        {
            message = error;
        }

        private string message;
        public override string Message
        {
            get { return message; }
        }
    }
}