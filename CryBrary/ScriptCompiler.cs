using System;
using System.Linq;
using System.IO;

using System.Collections.ObjectModel;
using System.Collections.Generic;

using System.CodeDom.Compiler;
using System.Reflection;

using CryEngine.Extensions;
using CryEngine.Utils;

using System.Xml.Linq;
using System.ComponentModel;

using System.Threading.Tasks;
using System.Diagnostics;

namespace CryEngine
{
	public static partial class ScriptCompiler
	{
		public static void Initialize()
		{
#if !RELEASE
			//Pdb2Mdb.Driver.Convert(Assembly.GetExecutingAssembly());
#endif

			CompiledScripts = new List<CryScript>();
			FlowNodes = new List<StoredNode>();

            assemblyReferenceHandler = new AssemblyReferenceHandler();

			NextScriptId = 0;

			//GenerateScriptbindAssembly(scriptBinds.ToArray());

            assemblyReferenceHandler.ReferencedAssemblies.AddRange(System.AppDomain.CurrentDomain.GetAssemblies().Select(a => a.Location));

			LoadPrecompiledAssemblies();

			AddScripts(CompileScriptsInFolders(
				PathUtils.GetScriptFolder(ScriptType.Entity),
				PathUtils.GetScriptFolder(ScriptType.GameRules),
				PathUtils.GetScriptFolder(ScriptType.FlowNode)
			));
		}

		public static void PostInit()
		{
			// These have to be registered later on due to the flow system being initialized late.
			RegisterFlownodes();
		}

		private static void LoadPrecompiledAssemblies()
		{
			//Add pre-compiled assemblies / plugins
			AddScripts(ScriptCompiler.LoadLibrariesInFolder(Path.Combine(PathUtils.GetScriptsFolder(), "Plugins")));
		}

		private static void AddScripts(IEnumerable<CryScript> scripts)
		{
			if(scripts == null || scripts.Count() < 1)
				return;

			CompiledScripts.AddRange(scripts);
		}

		/// <summary>
		/// Instantiates a script using its name and interface.
		/// </summary>
		/// <param name="scriptName"></param>
		/// <param name="constructorParams"></param>
		/// <returns>New instance scriptId or -1 if instantiation failed.</returns>
		[EditorBrowsable(EditorBrowsableState.Never)]
		public static CryScriptInstance InstantiateScript(string scriptName, object[] constructorParams = null)
		{
			if(scriptName.Length < 1)
			{
				Debug.LogAlways("Empty script passed to InstantiateClass");

				return null;
			}

			// I can play with sexy lambdas too!
			int index = CompiledScripts.FindIndex(x => x.ClassName.Equals(scriptName));
			if(index == -1)
			{
				Debug.LogAlways("Failed to instantiate {0}, compiled script could not be found.", scriptName);

				return null;
			}

			CryScript script = CompiledScripts.ElementAt(index);

			NextScriptId++;

			if(script.ScriptType == ScriptType.GameRules)
				script.ScriptInstances = null;

			if(script.ScriptInstances == null)
				script.ScriptInstances = new Collection<CryScriptInstance>();

			script.ScriptInstances.Add(System.Activator.CreateInstance(script.ClassType, constructorParams) as CryScriptInstance);
			script.ScriptInstances.Last().ScriptId = NextScriptId;

			CompiledScripts[index] = script;

			return script.ScriptInstances.Last();
		}

		/// <summary>
		/// Adds an script instance to the script collection and returns its new id.
		/// </summary>
		/// <param name="instance"></param>
		public static int AddScriptInstance(CryScriptInstance instance)
		{
			int scriptIndex = CompiledScripts.FindIndex(x => x.ClassName == instance.GetType().Name);
			if(scriptIndex != -1)
			{
				CryScript script = CompiledScripts.ElementAt(scriptIndex);

				if(script.ScriptInstances == null)
					script.ScriptInstances = new Collection<CryScriptInstance>();
				else if(script.ScriptInstances.Contains(instance))
					return -1;

				NextScriptId++;

				instance.ScriptId = NextScriptId;
				script.ScriptInstances.Add(instance);

				CompiledScripts[scriptIndex] = script;

				return NextScriptId;
			}

			Debug.LogAlways("Couldn't add script {0}", instance.GetType().Name);
			return -1;
		}

		/// <summary>
		/// Locates and destructs the script with the assigned scriptId.
		/// 
		/// Warning: Not supplying the scriptName will make the method execute slower.
		/// </summary>
		/// <param name="scriptId"></param>
		/// <param name="scriptName"></param>
		public static void RemoveInstance(int scriptId, string scriptName = "")
		{
			if(scriptName.Length > 0)
			{
				int index = CompiledScripts.FindIndex(x => x.ClassName.Equals(scriptName));
				if(index == -1)
					return;

				CryScript script = CompiledScripts[index];

				if(script.ScriptInstances != null)
				{
					var scriptInstance = script.ScriptInstances.First(x => x.ScriptId == scriptId);
					if(scriptInstance == null)
						return;

					int instanceIndex = script.ScriptInstances.IndexOf(scriptInstance);
					if(instanceIndex == -1)
					{
						Debug.LogAlways("Failed to remove script of type {0} with id {1}; instance was not found.", scriptName, scriptId);
						return;
					}

					script.ScriptInstances.RemoveAt(instanceIndex);

					CompiledScripts[index] = script;
				}
			}
			else
			{
				for(int i = 0; i < CompiledScripts.Count; i++)
				{
					CryScript script = CompiledScripts[i];

					if(script.ScriptInstances != null)
					{
						var scriptInstance = script.ScriptInstances.First(x => x.ScriptId == scriptId);
						if(scriptInstance == null)
							return;

						int scriptIndex = script.ScriptInstances.IndexOf(scriptInstance);
						if(scriptIndex != -1)
						{
							script.ScriptInstances.RemoveAt(scriptIndex);

							CompiledScripts[i] = script;

							break;
						}
					}
				}
			}
		}

		[EditorBrowsable(EditorBrowsableState.Never)]
		public static object InvokeScriptFunctionById(int id, string func, object[] args = null)
		{
			CryScriptInstance scriptInstance = GetScriptInstanceById(id);
			if(scriptInstance == default(CryScriptInstance))
			{
				Debug.LogAlways("Failed to invoke method, script instance was invalid");
				return null;
			}

			return InvokeScriptFunction(scriptInstance, func, args);
		}

		public static CryScriptInstance GetScriptInstanceById(int id)
		{
			var scripts = CompiledScripts.Where(script => script.ScriptInstances != null);

			CryScriptInstance scriptInstance = null;
			foreach(var script in scripts)
			{
				scriptInstance = script.ScriptInstances.FirstOrDefault(instance => instance.ScriptId == id);

				if(scriptInstance != default(CryScriptInstance))
					return scriptInstance;
			}

			return null;
		}

		public static int GetEntityScriptId(EntityId entityId, System.Type scriptType = null)
		{
			var scripts = CompiledScripts.Where(script => (scriptType != null ? script.ClassType.Implements(scriptType) : true) && script.ScriptInstances != null);

			foreach(var compiledScript in scripts)
			{
				foreach(var script in compiledScript.ScriptInstances)
				{
					var scriptEntity = script as StaticEntity;
					if(scriptEntity != null && scriptEntity.Id == entityId)
						return script.ScriptId;
				}
			}

			return -1;
		}

		/// <summary>
		/// Automagically registers scriptbind methods to rid us of having to add them in both C# and C++.
		/// </summary>
		[EditorBrowsable(EditorBrowsableState.Never)]
		public static void RegisterScriptbind(string namespaceName, string className, object[] methods)
		{
			if(ScriptBinds == null)
				ScriptBinds = new List<Scriptbind>();

			ScriptBinds.Add(new Scriptbind(namespaceName, className, methods));
		}

		/// <summary>
		/// Called once per frame.
		/// </summary>
		public static void OnUpdate(float frameTime)
		{
			Time.DeltaTime = frameTime;

			Parallel.ForEach(CompiledScripts, script =>
			{
				if(script.ScriptInstances != null)
					script.ScriptInstances.Where(i => i.ReceiveUpdates).ToList().ForEach(i => i.OnUpdate());
			});

			EntitySystem.OnUpdate();
		}

		/// <summary>
		/// This function will automatically scan for C# dll (*.dll) files and load the types contained within them.
		/// </summary>
		public static IEnumerable<CryScript> LoadLibrariesInFolder(string directory)
		{
			if (!Directory.Exists(directory))
			{
				Debug.LogAlways("Libraries failed to load; Folder {0} does not exist.", directory);
				return null;
			}

			var plugins = Directory.GetFiles(directory, "*.dll", SearchOption.AllDirectories);

			if (plugins != null && plugins.Length != 0)
			{
				List<CryScript> compiledScripts = new List<CryScript>();

				foreach (var plugin in plugins)
				{
					try
					{
						string newPath = Path.Combine(Path.GetTempPath(), Path.GetFileName(plugin));

						File.Copy(plugin, newPath, true);
#if !RELEASE
						GenerateDebugDatabaseForAssembly(plugin);

						string mdbFile = plugin + ".mdb";
						if (File.Exists(mdbFile)) // success
							File.Copy(mdbFile, Path.Combine(Path.GetTempPath(), Path.GetFileName(mdbFile)), true);
#endif

						//Process it, in case it contains types/gamerules
						Assembly assembly = Assembly.LoadFrom(newPath);

                        assemblyReferenceHandler.ReferencedAssemblies.Add(newPath);

						compiledScripts.AddRange(LoadAssembly(assembly));
					}
					//This exception tells us that the assembly isn't a valid .NET assembly for whatever reason
					catch (BadImageFormatException)
					{
						Debug.LogAlways("Plugin loading failed for {0}; dll is not valid.", plugin);
					}
				}

				return compiledScripts.ToArray();
			}

			return null;
		}

		/// <summary>
		/// This function will automatically scan for C# (*.cs) files and compile them using CompileScripts.
		/// </summary>
		public static IEnumerable<CryScript> CompileScriptsInFolder(string directory)
		{
			if (!Directory.GetParent(directory).Exists)
			{
				Debug.LogAlways("Aborting script compilation; script directory parent could not be located.");
				return null;
			}

			if (!Directory.Exists(directory))
			{
				Debug.LogAlways("Script compilation failed; Folder {0} does not exist.", directory);
				return null;
			}

			string[] scriptsInFolder = Directory.GetFiles(directory, "*.cs", SearchOption.AllDirectories);
			if (scriptsInFolder == null || scriptsInFolder.Length < 1)
			{
				Debug.LogAlways("No scripts were found in {0}.", directory);
				return null;
			}

			return CompileScripts(scriptsInFolder, ScriptLanguage.CSharp);
		}

		public static IEnumerable<CryScript> CompileScriptsInFolders(params string[] scriptFolders)
		{
			List<string> scripts = new List<string>();
			foreach (var directory in scriptFolders)
			{
				if (Directory.Exists(directory))
					scripts.AddRange(Directory.GetFiles(directory, "*.cs", SearchOption.AllDirectories));
				else
					Debug.LogAlways("Could not compile scripts in {0}; directory not found", directory);
			}

			if (scripts.Count > 0)
				return CompileScripts(scripts.ToArray(), ScriptLanguage.CSharp);
			else
				return null;
		}

		public enum ScriptLanguage
		{
			CSharp,
			VisualBasic,
			JScript
		}

		/// <summary>
		/// Compiles the scripts and compiles them into an assembly.
		/// </summary>
		/// <param name="scripts">A string array containing full paths to scripts to be compiled.</param>
		/// <returns></returns>
		public static IEnumerable<CryScript> CompileScripts(string[] scripts, ScriptLanguage language)
		{
			if (scripts.Length < 1)
				return null;

			CodeDomProvider provider = null;
			if(language == ScriptLanguage.VisualBasic)
				provider = CodeDomProvider.CreateProvider("VisualBasic");
			else if(language == ScriptLanguage.JScript)
				provider = CodeDomProvider.CreateProvider("JScript");
			else if(language == ScriptLanguage.CSharp)
				provider = CodeDomProvider.CreateProvider("CSharp");

			CompilerParameters compilerParameters = new CompilerParameters();

			compilerParameters.GenerateExecutable = false;
			compilerParameters.GenerateInMemory = true;

			//Add additional assemblies as needed by gamecode to referencedAssemblies
            foreach (var assembly in assemblyReferenceHandler.GetRequiredAssembliesForScripts(scripts))
			{
				if (!compilerParameters.ReferencedAssemblies.Contains(assembly))
					compilerParameters.ReferencedAssemblies.Add(assembly);
			}

            compilerParameters.ReferencedAssemblies.AddRange(assemblyReferenceHandler.ReferencedAssemblies.ToArray());

#if RELEASE
			compilerParameters.IncludeDebugInformation = false;
#else
			// Necessary for stack trace line numbers etc
			compilerParameters.IncludeDebugInformation = true;
#endif
			try
			{
				CompilerResults results = provider.CompileAssemblyFromFile(compilerParameters, scripts);

				provider.Dispose();
				provider = null;
				compilerParameters = null;

				if (results.CompiledAssembly != null) // success
					return LoadAssembly(results.CompiledAssembly);
				else if (results.Errors.HasErrors)
				{
					Debug.LogAlways("Compilation failed; {0} errors:", results.Errors.Count);

					foreach (CompilerError error in results.Errors)
						Debug.LogAlways(error.ErrorText);
				}
				else
					throw new ArgumentNullException("Tried loading a NULL assembly");
			}
			catch (Exception ex)
			{
				Debug.LogException(ex);
			}

			return null;
		}


		/// <summary>
		/// Loads an C# assembly and return encapulsated script Type.
		/// </summary>
		public static IEnumerable<CryScript> LoadAssembly(Assembly assembly)
		{
			var assemblyTypes = assembly.GetTypes().Where(type => type.Implements(typeof(CryScriptInstance)));

			List<CryScript> scripts = new List<CryScript>();

			Parallel.For(0, assemblyTypes.Count(), i =>
			{
				var type = assemblyTypes.ElementAt(i);

				if (type != null && !type.ContainsAttribute<ExcludeFromCompilationAttribute>())
				{
					scripts.Add(new CryScript(type));

					if (type.Implements(typeof(BaseGameRules)))
					{
						scripts.Last().SetScriptType(ScriptType.GameRules);

						GameRulesSystem._RegisterGameMode(scripts.Last().ClassName);

						if (type.ContainsAttribute<DefaultGamemodeAttribute>())
							GameRulesSystem._SetDefaultGameMode(scripts.Last().ClassName);
					}
					else if (type.Implements(typeof(BasePlayer)))
					{
						scripts.Last().SetScriptType(ScriptType.Actor);

						ActorSystem._RegisterActorClass(scripts.Last().ClassName, false);
					}
					else if (type.Implements(typeof(StaticEntity)))
					{
						bool staticEntity = !type.Implements(typeof(Entity));

						if (!staticEntity)
							scripts.Last().SetScriptType(ScriptType.Entity);
						else
							scripts.Last().SetScriptType(ScriptType.StaticEntity);

						LoadEntity(type, scripts.Last(), staticEntity);
					}
					else if (type.Implements(typeof(FlowNode)))
					{
						scripts.Last().SetScriptType(ScriptType.FlowNode);

						LoadFlowNode(type, scripts.Last().ClassName);
					}
					else if (type.Implements(typeof(CryScriptInstance)))
						scripts.Last().SetScriptType(ScriptType.Unknown);
				}
			});

			assemblyTypes = null;

			return scripts.ToArray();
		}

		internal struct StoredNode
		{
			public StoredNode(string Class, string Category)
				: this()
			{
				className = Class;
				category = Category;
			}

			public string className;
			public string category;
		}

		internal static void RegisterFlownodes()
		{
			foreach (var node in FlowNodes)
				FlowSystem.RegisterNode(node.className, node.category, node.category.Equals("entity", StringComparison.Ordinal));
		}

		private static void LoadEntity(Type type, CryScript script, bool staticEntity)
		{
			// Shoulda thought of this before. Abstract classes brutally murder the engine otherwise.
			if (type.IsAbstract)
				return;

			EntityConfig config = default(EntityConfig);
			StaticEntity entity = null;

			if (staticEntity)
				entity = Activator.CreateInstance(type) as StaticEntity;
			else
				entity = Activator.CreateInstance(type) as Entity;

			config = entity.GetEntityConfig();

			entity = null;

			if (config.registerParams.Name.Length <= 0)
				config.registerParams.Name = script.ClassName;
			if (config.registerParams.Category.Length <= 0)
				config.registerParams.Category = ""; // TODO: Use the folder structure in Scripts/Entities. (For example if the entity is in Scripts/Entities/Multiplayer, the category should become "Multiplayer")

			EntitySystem.RegisterEntityClass(config);

			LoadFlowNode(type, config.registerParams.Name, true);
		}

		private static void LoadFlowNode(Type type, string nodeName, bool entityNode = false)
		{
			string category = null;

			if (!entityNode)
			{
				category = type.Namespace;

				FlowNodeAttribute nodeInfo;
				if (type.TryGetAttribute<FlowNodeAttribute>(out nodeInfo))
				{
					if (nodeInfo.UICategory != null)
						category = nodeInfo.UICategory;

					if (nodeInfo.Name != null)
						nodeName = nodeInfo.Name;
				}
			}
			else
				category = "entity";

			FlowNodes.Add(new StoredNode(nodeName, category));
		}

		public static object InvokeScriptFunction(object scriptInstance, string func, object[] args = null)
		{
			if (scriptInstance == null)
			{
				Debug.LogAlways("Attempted to invoke method {0} with an invalid instance.", func);
				return null;
			}

			var methodInfo = scriptInstance.GetType().GetMethods().First(method =>
			{
				var parameters = method.GetParameters();

				if (method.Name == func)
				{
					if ((parameters == null || parameters.Length == 0) && args == null)
						return true;
					else if (parameters.Length == args.Length)
					{
						for (int i = 0; i < args.Length; i++)
						{
							if (parameters[i].ParameterType == args[i].GetType())
								return true;
						}
					}
				}

				return false;
			});

			if (methodInfo == null)
			{
				Debug.LogAlways("Could not find method {0} in type {1}", func, scriptInstance.GetType().ToString());
				return null;
			}

			// Sort out optional parameters
			ParameterInfo[] info = methodInfo.GetParameters();

			if (info.Length > 0)
			{
				object[] tempArgs;
				tempArgs = new object[info.Length];
				int argIndexLength = args.Length - 1;

				for (int i = 0; i < info.Length; i++)
				{
					if (i <= argIndexLength)
						tempArgs.SetValue(args[i], i);
					else if (i > argIndexLength && info[i].IsOptional)
						tempArgs[i] = info[i].DefaultValue;
				}

				args = null;
				args = tempArgs;
				tempArgs = null;
			}
			else
				args = null;

			object result = methodInfo.Invoke(scriptInstance, args);

			args = null;
			methodInfo = null;
			info = null;

			return result;
		}

		public static void GenerateDebugDatabaseForAssembly(string assemblyPath)
		{
			if (File.Exists(Path.ChangeExtension(assemblyPath, "pdb")))
			{
				Assembly assembly = Assembly.LoadFrom(Path.Combine(PathUtils.GetEngineFolder(), "Mono", "bin", "pdb2mdb.dll"));
				Type driver = assembly.GetType("Driver");
				MethodInfo convertMethod = driver.GetMethod("Convert", BindingFlags.Static | BindingFlags.Public);

				object[] args = { assemblyPath };
				convertMethod.Invoke(null, args);
			}
		}

		internal static List<StoredNode> FlowNodes;
        private static AssemblyReferenceHandler assemblyReferenceHandler;

		static List<Scriptbind> ScriptBinds;

		public static List<CryScript> CompiledScripts;
		public static int NextScriptId;
	}

	public enum ScriptType
	{
		Null = -1,
		/// <summary>
		/// Scripts directly inheriting from BaseGameRules will utilize this script type.
		/// </summary>
		GameRules,
		/// <summary>
		/// Scripts directly inheriting from FlowNode will utilize this script type.
		/// </summary>
		FlowNode,
		/// <summary>
		/// Scripts directly inheriting from StaticEntity will utilize this script type.
		/// </summary>
		StaticEntity,
		/// <summary>
		/// Scripts directly inheriting from Entity will utilize this script type.
		/// </summary>
		Entity,
		/// <summary>
		/// Scripts directly inheriting from Actor will utilize this script type.
		/// </summary>
		Actor,
		/// <summary>
		/// 
		/// </summary>
		EditorForm,
		/// <summary>
		/// Scripts will be linked to this type if they inherit from CryScriptInstance, but not any other script base.
		/// </summary>
		Unknown
	}

	/// <summary>
	/// Represents a given class.
	/// </summary>
	public struct CryScript
	{
		public CryScript(Type type)
			: this()
		{
			ClassType = type;
			ClassName = ClassType.Name;
		}

		public void SetScriptType(ScriptType scriptType)
		{
			ScriptType = scriptType;
		}

		public Type ClassType { get; private set; }
		public ScriptType ScriptType { get; private set; }

		// Type.Name is costly to call
		public string ClassName { get; private set; }

		/// <summary>
		/// Stores all instances of this class.
		/// </summary>
		public Collection<CryScriptInstance> ScriptInstances { get; internal set; }

		#region Operators
		public static bool operator ==(CryScript script1, CryScript script2)
		{
			return script1.ClassType == script2.ClassType;
		}

		public static bool operator !=(CryScript script1, CryScript script2)
		{
			return script1.ClassType != script2.ClassType;
		}

		public override bool Equals(object obj)
		{
			if (obj is CryScript)
				return (CryScript)obj == this;

			return false;
		}

		public override int GetHashCode()
		{
			return ClassType.GetHashCode();
		}

		#endregion
	}

	public struct InternalCallMethod
	{
		public string name;
		public string returnType;

		public string parameters;
	}

	public struct Scriptbind
	{
		public Scriptbind(string Namespace, string Class, object[] Methods)
			: this()
		{
			namespaceName = Namespace;
			className = Class;

			methods = Methods;
		}

		public string namespaceName;
		public string className;

		/// <summary>
		/// Array of InternalCallMethod
		/// </summary>
		public object[] methods;
	}
}