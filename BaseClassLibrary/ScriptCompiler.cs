using System;
using System.Linq;
using System.IO;
using System.Collections.Generic;

using System.CodeDom.Compiler;
using Microsoft.CSharp;
using System.Reflection;

using CryEngine.Extensions;

/// <summary>The main engine namespace, otherwise known as the CryENGINE3 Base Class Library.</summary>
namespace CryEngine
{
	/// <summary>
	/// The script compiler is responsible for all CryScript compilation.
	/// </summary>
    public static class ScriptCompiler
    {
        /// <summary>
        /// This function will automatically scan for C# dll (*.dll) files and load the types contained within them.
        /// </summary>
        public static CryScript[] LoadLibrariesInFolder(string directory)
        {
            if (!Directory.Exists(directory))
            {
                CryConsole.LogAlways("Libraries failed to load; Folder {0} does not exist.", directory);
                return null;
            }
            
            var plugins = Directory.GetFiles(directory, "*.dll", SearchOption.AllDirectories);
            if (plugins != null || plugins.Length != 0)
            {
                foreach (var plugin in plugins)
                {
                    try
                    {
                        AssemblyName assemblyName = AssemblyName.GetAssemblyName(plugin);

                        //Process it, in case it contains types/gamerules
                        Assembly assembly = Assembly.LoadFrom(plugin);
                        return LoadAssembly(assembly);
                    }
                    //This exception tells us that the assembly isn't a valid .NET assembly for whatever reason
                    catch (BadImageFormatException)
                    {
                        CryConsole.LogAlways("Plugin loading failed for {0}; dll is not valid.", plugin);
                    }
                }
            }
            else
                CryConsole.LogAlways("No plugins detected.");
            
            return null;
        }

        /// <summary>
        /// This function will automatically scan for C# (*.cs) files and compile them to an assembly.
        /// </summary>
        public static CryScript[] CompileScriptsInFolder(string directory)
        {
            if (!Directory.Exists(directory))
            {
                CryConsole.LogAlways("Script compilation failed; Folder {0} does not exist.", directory);
                return null;
            }

            string[] scriptsInFolder = Directory.GetFiles(directory, "*.cs", SearchOption.AllDirectories);
            if (scriptsInFolder == null || scriptsInFolder.Length < 1)
            {
                CryConsole.LogAlways("No scripts were found in {0}.", directory);
                return null;
            }

            CodeDomProvider provider = new CSharpCodeProvider();
            CompilerParameters compilerParameters = new CompilerParameters();

            compilerParameters.GenerateExecutable = false;
            compilerParameters.GenerateInMemory = true;

#if DEBUG
            compilerParameters.IncludeDebugInformation = true;
#else
            compilerParameters.IncludeDebugInformation = false;
#endif

            //Automatically add needed assemblies
            var assemblies = AppDomain.CurrentDomain.GetAssemblies().Select(a => a.Location);
            foreach (var assemblyPath in assemblies)
                compilerParameters.ReferencedAssemblies.Add(assemblyPath);

            CompilerResults results = null;
            try
            {
                results = provider.CompileAssemblyFromFile(compilerParameters, scriptsInFolder);

                if (results.CompiledAssembly != null) // success
                    return LoadAssembly(results.CompiledAssembly);
                else if (results.Errors.HasErrors)
                {
                    CryConsole.LogAlways("Compilation failed; {0} errors:", results.Errors.Count);

                    foreach (CompilerError error in results.Errors)
                        CryConsole.LogAlways(error.ErrorText);
                }
                else
                    throw new ArgumentNullException("Tried loading a NULL assembly");
            }
            catch (Exception ex)
            {
                CryConsole.LogException(ex);
            }

            return null;
        }

        /// <summary>
        /// Loads an C# assembly and return encapulsated script Type.
        /// </summary>
        public static CryScript[] LoadAssembly(Assembly assembly)
        {
            Type[] assemblyTypes = assembly.GetTypes();

            CryScript[] scripts = new CryScript[assemblyTypes.Length];

            //Extract the types and load everything that implements IEntity, IGameRules and IFlowNode
            for (var i = 0; i < scripts.Length; i++)
            {
                Type type = assemblyTypes[i]; 

                if (!type.ContainsAttribute<ExcludeFromCompilationAttribute>())
                {
                    MonoScriptType scriptType = MonoScriptType.Null;
                    // TODO: Add some kind of flag devs can use to make sure their game mode doesn't get registered. (To avoid doing if(type.Name!=BaseGameRules) etc)
                    // Ruan edit: See above. I'm a genius.
                    if (type.Implements(typeof(IGameRules)))
                        scriptType = MonoScriptType.GameRules;
                    else if (type.Implements(typeof(Entity)))
                        scriptType = MonoScriptType.Entity;
                    /*else if (typeof(IFlowNode).IsAssignableFrom(type))
                        FlowSystem.RegisterNode(type.Name);*/

                    scripts[i] = new CryScript(type, scriptType);

                    // This is done after CryScript construction to avoid calling Type.name several times
                    if (scriptType == MonoScriptType.GameRules)
                    {
                        GameRules.RegisterGameMode(scripts[i].className);

                        if (type.ContainsAttribute<DefaultGamemodeAttribute>())
                            GameRules.SetDefaultGameMode(scripts[i].className);
                    }
                    else if (scriptType == MonoScriptType.Entity)
                        LoadEntity(type, scripts[i]);
                }
            }

            return scripts;
        }

        private static void LoadEntity(Type type, CryScript script)
        {
			CryConsole.LogAlways("Registering new entity: {0}", script.className);

			Entity entity = Activator.CreateInstance(type) as Entity;
			EntityConfig config = entity.GetConfig();

            if (config.registerParams.Name.Length <= 0)
                config.registerParams.Name = script.className;
            if (config.registerParams.Category.Length <= 0)
                config.registerParams.Category = ""; // TODO: Use the folder structure in Scripts/Entities. (For example if the entity is in Scripts/Entities/Multiplayer, the category should become "Multiplayer")

            EntitySystem.RegisterEntityClass(config);
        }
    }

    public enum MonoScriptType
    {
        Null,
        GameRules,
        FlowNode,
        Entity
    }

	/// <summary>
	/// Represents a given class.
	/// </summary>
    public struct CryScript
    {
        public CryScript(Type _type, MonoScriptType type)
            : this()
        {
            Type = _type;
            ScriptType = type;

            className = Type.Name;
        }

        public Type Type { get; private set; }
        public MonoScriptType ScriptType { get; private set; }

        // Type.Name is costly to call
        public string className { get; private set; }

		/// <summary>
		/// Stores all instances of this class.
		/// </summary>
        public List<CryScriptInstance> Instances { get; set; }
    }

	/// <summary>
	/// Represents a given instance of a CryScript.
	/// </summary>
    public struct CryScriptInstance
    {
        public CryScriptInstance(int id, object instance)
            : this()
        {
            Instance = instance;
            m_id = id;
        }

        private int m_id;

        public int Id
        {
            get { return m_id; }
            // this may only be set once
            internal set { m_id = value; }
        }

        public object Instance { get; set; }
    }
}
