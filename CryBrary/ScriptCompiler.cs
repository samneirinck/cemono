using System;
using System.Linq;
using System.IO;

using System.Collections.ObjectModel;
using System.Collections.Generic;

using System.CodeDom.Compiler;
using System.Reflection;

using CryEngine.Extensions;
using CryEngine.Utilities;

using System.ComponentModel;

using System.Threading.Tasks;

namespace CryEngine
{
	public static partial class ScriptCompiler
	{
		public static bool Initialize()
		{
			CompiledScripts = new Collection<CryScript>();
			FlowNodes = new Collection<StoredNode>();

            assemblyReferenceHandler = new AssemblyReferenceHandler();

			LastScriptId = 0;

			LoadPrecompiledAssemblies();

			var compilationParams = new CompilationParameters();

			compilationParams.Folders = new string[] 
			{ 
				PathUtils.GetScriptFolder(ScriptType.Entity),
				PathUtils.GetScriptFolder(ScriptType.GameRules),
				PathUtils.GetScriptFolder(ScriptType.FlowNode),
				PathUtils.GetScriptFolder(ScriptType.UIEvent)
			};

			try
			{
				CompileScripts(ref compilationParams);
			}
			catch(ScriptCompilationException ex)
			{
				Debug.LogException(ex);

				return false;
			}

			return true;
		}

		public static void PostInit()
		{
			// These have to be registered later on due to the flow system being initialized late.
			RegisterFlownodes();
		}

		private static void LoadPrecompiledAssemblies()
		{
			//Add pre-compiled assemblies / plugins
			ScriptCompiler.LoadLibrariesInFolder(Path.Combine(PathUtils.GetScriptsFolder(), "Plugins"));
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
				throw new ArgumentException("Empty script name passed to InstantiateClass");

            var script = CompiledScripts.FirstOrDefault(x => x.ScriptType.Name.Equals(scriptName));
            if(script == default(CryScript))
				throw new ScriptNotFoundException(string.Format("Compiled script {0} could not be found.", scriptName));

            int index = CompiledScripts.IndexOf(script);

			AddScriptInstance(System.Activator.CreateInstance(CompiledScripts[index].ScriptType, constructorParams) as CryScriptInstance, index);

			return CompiledScripts[index].ScriptInstances.Last();
		}

		/// <summary>
		/// Adds an script instance to the script collection and returns its new id.
		/// </summary>
		/// <param name="instance"></param>
		public static int AddScriptInstance(CryScriptInstance instance)
		{
			if(instance == null)
				return -1;

            var script = CompiledScripts.FirstOrDefault(x => x.ScriptType == instance.GetType());
            if (script == default(CryScript))
                throw new ScriptNotFoundException(string.Format("Compiled script {0} could not be found.", instance.GetType().Name));

            return AddScriptInstance(instance, CompiledScripts.IndexOf(script));
		}

		/// <summary>
		/// Adds an script instance to the script collection and returns its new id.
		/// </summary>
		/// <param name="instance"></param>
		/// <param name="scriptIndex">Index of the CryScript object residing in ScriptCompiler.CompiledScripts</param>
		public static int AddScriptInstance(CryScriptInstance instance, int scriptIndex)
		{
			if(scriptIndex == -1)
				throw new ScriptNotFoundException(string.Format("Failed to locate the compiled script of type {0}", instance.GetType().Name));

			var script = CompiledScripts.ElementAt(scriptIndex);

			if(script.ScriptInstances == null)
				script.ScriptInstances = new Collection<CryScriptInstance>();
			else if(script.ScriptInstances.Contains(instance))
				return -1;

			LastScriptId++;

			instance.ScriptId = LastScriptId;
			script.ScriptInstances.Add(instance);

			CompiledScripts[scriptIndex] = script;

			return LastScriptId;
		}

		public static void RemoveInstance(int scriptId)
		{
			RemoveInstance(scriptId, null);
		}

		/// <summary>
		/// Locates and destructs the script with the assigned scriptId.
		/// </summary>
		/// <param name="scriptId"></param>
		public static void RemoveInstance(int scriptId, Type scriptType)	
		{
			if(scriptType != null)
			{
				var script = CompiledScripts.FirstOrDefault(x => x.ScriptType == scriptType);
				if(script == default(CryScript))
					throw new ScriptNotFoundException(string.Format("Failed to find script by name {0}", scriptType.Name));

				RemoveInstanceFromScriptById(ref script, scriptId);

				CompiledScripts[CompiledScripts.IndexOf(script)] = script;
			}
			else
			{
				for(int i = 0; i < CompiledScripts.Count; i++)
				{
					CryScript script = CompiledScripts[i];

					RemoveInstanceFromScriptById(ref script, scriptId);

					CompiledScripts[i] = script;
				}
			}
		}

		static void RemoveInstanceFromScriptById(ref CryScript script, int scriptId)
		{
			if(script.ScriptInstances != null && script.ScriptInstances.Count > 0)
			{
				var scriptInstance = script.ScriptInstances.FirstOrDefault(x => x.ScriptId == scriptId);
				if(scriptInstance == InvalidScriptInstance)
					return;

				int instanceIndex = script.ScriptInstances.IndexOf(scriptInstance);
				if(instanceIndex == -1)
				{
					Debug.LogAlways("Failed to remove script with id {0}; instance was not found.", scriptId);
					return;
				}

				script.ScriptInstances.RemoveAt(instanceIndex);
			}
		}

		public static CryScriptInstance GetScriptInstanceById(int id)
		{
			var scripts = CompiledScripts.Where(script => script.ScriptInstances != null && script.ScriptInstances.Count > 0);

			CryScriptInstance scriptInstance = null;
			foreach(var script in scripts)
			{
				scriptInstance = script.ScriptInstances.FirstOrDefault(instance => instance.ScriptId == id);

				if(scriptInstance != InvalidScriptInstance)
					return scriptInstance;
			}

			return null;
		}

		/// <summary>
		/// Avoid creating a new empty CryScriptInstance each time we need to check
		/// </summary>
		static CryScriptInstance InvalidScriptInstance = default(CryScriptInstance);

		public static int GetEntityScriptId(EntityId entityId, System.Type scriptType = null)
		{
			var scripts = CompiledScripts.Where(script => (scriptType != null ? script.ScriptType.Implements(scriptType) : true) && script.ScriptInstances != null);

			foreach(var compiledScript in scripts)
			{
				foreach(var script in compiledScript.ScriptInstances)
				{
					var scriptEntity = script as Entity;
					if(scriptEntity != null && scriptEntity.Id == entityId)
						return script.ScriptId;
				}
			}

			return -1;
		}

		/// <summary>
		/// Called once per frame.
		/// </summary>
		public static void OnUpdate(float frameTime)
		{
			Time.DeltaTime = frameTime;

			foreach(var script in CompiledScripts)
			{
				if(script.ScriptInstances != null)
				{
					foreach(var instance in script.ScriptInstances)
						if(instance.ReceiveUpdates)
							instance.OnUpdate();
				}
			}

			Entity.UpdateSpawnedEntities();
		}

		/// <summary>
		/// This function will automatically scan for C# dll (*.dll) files and load the types contained within them.
		/// </summary>
		public static void LoadLibrariesInFolder(string directory)
		{
			if (!Directory.Exists(directory))
				throw new DirectoryNotFoundException(string.Format("Libraries failed to load; Folder {0} does not exist.", directory));

			var plugins = Directory.GetFiles(directory, "*.dll", SearchOption.AllDirectories);

			if (plugins != null && plugins.Length != 0)
			{
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
						LoadAssembly(Assembly.LoadFrom(newPath));
					}
					//This exception tells us that the assembly isn't a valid .NET assembly for whatever reason
					catch (BadImageFormatException)
					{
						Debug.LogAlways("Plugin loading failed for {0}; dll is not valid.", plugin);
					}
				}
			}
		}

		public enum ScriptLanguage
		{
			CSharp,
			VisualBasic,
			JScript
		}

		public class CompilationParameters
		{
			public CompilationParameters()
			{
				Language = ScriptLanguage.CSharp;
			}

			public ScriptLanguage Language { get; set; }
			public CompilerResults Results { get; internal set; }

			/// <summary>
			/// The folders from which scripts should be compiled from.
			/// </summary>
			public string[] Folders { get; set; }

			/// <summary>
			/// Forces generation of debug information, even in release mode.
			/// </summary>
			public bool ForceDebugInformation { get; set; }
		}

		/// <summary>
		/// Compiles the scripts and compiles them into an assembly.
		/// </summary>
		/// <param name="compilationParameters"></param>
		/// <returns></returns>
		/// <exception cref="System.IO.DirectoryNotFoundException">Thrown when one or more script folders could not be located.</exception>
		/// <exception cref="System.ArgumentNullException">Thrown if the compiled assembly could not be loaded.</exception>
		/// <exception cref="CryEngine.ScriptCompilationException">Thrown if one or more compilation errors occur.</exception>
		public static void CompileScripts(ref CompilationParameters compilationParameters)
		{
            if (compilationParameters == null)
                throw new ArgumentException("CompilationParameters was null");

			if(compilationParameters.Folders == null)
                throw new ArgumentException(message: "Supplied Folders array in CompilationParameters argument was null");
			else if(compilationParameters.Folders.Length < 1)
                throw new ArgumentException(message: "Supplied Folders array in CompilationParameters did not contain any strings");

			var scripts = new List<string>();
			foreach(var directory in compilationParameters.Folders)
			{
				if (Directory.Exists(directory))
					scripts.AddRange(Directory.GetFiles(directory, "*.cs", SearchOption.AllDirectories));
				else
					throw new DirectoryNotFoundException(message: string.Format("Could not compile scripts in {0}; directory not found", directory));
			}

			if(scripts.Count <= 0)
				return;

			CodeDomProvider provider = null;
			switch(compilationParameters.Language)
			{
				case ScriptLanguage.VisualBasic:
					provider = CodeDomProvider.CreateProvider("VisualBasic");
					break;
				case ScriptLanguage.JScript:
					provider = CodeDomProvider.CreateProvider("JScript");
					break;
				case ScriptLanguage.CSharp:
					provider = CodeDomProvider.CreateProvider("CSharp");
					break;
			}

			CompilerParameters compilerParameters = new CompilerParameters();

			compilerParameters.GenerateExecutable = false;

			bool includeDebugInfo = true;

#if RELEASE
			if(!compilationParams.ForceDebugInformation)
				includeDebugInfo = false;
#endif

			// Necessary for stack trace line numbers etc
			compilerParameters.IncludeDebugInformation = includeDebugInfo;
			compilerParameters.GenerateInMemory = !includeDebugInfo;

			//Add additional assemblies as needed by gamecode to referencedAssemblies
            foreach (var assembly in assemblyReferenceHandler.GetRequiredAssembliesForScriptFiles(scripts))
			{
				if (!compilerParameters.ReferencedAssemblies.Contains(assembly))
					compilerParameters.ReferencedAssemblies.Add(assembly);
			}

			compilerParameters.ReferencedAssemblies.AddRange(AppDomain.CurrentDomain.GetAssemblies().Select(x => x.Location).ToArray());

			CompilerResults results = provider.CompileAssemblyFromFile(compilerParameters, scripts.ToArray());

			provider.Dispose();
			provider = null;
			compilerParameters = null;

			compilationParameters.Results = results;

			if(results.CompiledAssembly != null) // success
				LoadAssembly(results.CompiledAssembly);
			else if(results.Errors.HasErrors)
			{
				string compilationError = string.Format("Compilation failed; {0} errors: ", results.Errors.Count);

				foreach(CompilerError error in results.Errors)
					compilationError += Environment.NewLine + string.Format("{0}({1},{2}): {3} {4}: {5}", error.FileName, error.Line, error.Column, error.IsWarning ? "warning" : "error", error.ErrorNumber, error.ErrorText);
				throw new ScriptCompilationException(compilationError);
			}
			else
                throw new ArgumentNullException(paramName: "Tried loading a NULL assembly");
		}


		/// <summary>
		/// Loads an C# assembly and adds all found types to ScriptCompiler.CompiledScripts
		/// </summary>
		public static void LoadAssembly(Assembly assembly)
		{
			var assemblyTypes = assembly.GetTypes().Where(type => type.Implements(typeof(CryScriptInstance)) || type.ContainsAttribute<UIEventAttribute>());

			Parallel.For(0, assemblyTypes.Count(), i =>
			{
				try
				{
					ProcessType(assemblyTypes.ElementAt(i));
				}
				catch(Exception ex)
				{
					Debug.LogException(ex);
				}
			});

			assemblyTypes = null;
		}

		/// <summary>
		/// Processes a type and adds all found types to ScriptCompiler.CompiledScripts
		/// </summary>
		/// <param name="type"></param>
		public static void ProcessType(Type type)
		{
			if(type != null && !type.ContainsAttribute<ExcludeFromCompilationAttribute>())
			{
				if(type.IsAbstract)
					throw new TypeLoadException(string.Format("Failed to load entity of type {0}: abstract entities are not supported", type.Name));

				CompiledScripts.Add(new CryScript(type));

				string className = type.Name;

				if(type.Implements(typeof(BaseGameRules)))
				{
					GameRulesSystem._RegisterGameMode(className);

					if(type.ContainsAttribute<DefaultGamemodeAttribute>())
						GameRulesSystem._SetDefaultGameMode(className);
				}
				else if(type.Implements(typeof(Actor)))
					Actor._RegisterActorClass(className, false);
				else if(type.Implements(typeof(Entity)))
				{
					bool staticEntity = !type.Implements(typeof(Entity));

					LoadEntity(type, CompiledScripts.Last(), staticEntity);
				}
				else if(type.Implements(typeof(FlowNode)))
					LoadFlowNode(type, className);
				else if(type.ContainsAttribute<UIEventAttribute>())
					UI.LoadEvent(type);
			}
		}

		internal static void RegisterFlownodes()
		{
			foreach (var node in FlowNodes)
				FlowNode.RegisterNode(node.className, node.category, node.category.Equals("entity"));
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="type"></param>
		/// <param name="script"></param>
		/// <param name="staticEntity"></param>
		/// <exception cref="System.TypeLoadException">Thrown if the type was invalid</exception>
		private static void LoadEntity(Type type, CryScript script, bool staticEntity)
		{
			EntityConfig config = Entity.GetEntityConfig(type);

			if (config.registerParams.Name.Length <= 0)
				config.registerParams.Name = script.ScriptType.Name;
			if (config.registerParams.Category.Length <= 0)
				config.registerParams.Category = ""; // TODO: Use the folder structure in Scripts/Entities. (For example if the entity is in Scripts/Entities/Multiplayer, the category should become "Multiplayer")

			Entity.RegisterEntityClass(config);

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

		internal static Collection<StoredNode> FlowNodes;
        private static AssemblyReferenceHandler assemblyReferenceHandler;

		public static Collection<CryScript> CompiledScripts;
		/// <summary>
		/// Last assigned ScriptId, next = + 1
		/// </summary>
		public static int LastScriptId;
	}

	public enum ScriptType
	{
		/// <summary>
		/// Scripts will be linked to this type if they inherit from CryScriptInstance, but not any other script base.
		/// </summary>
		Unknown = 0,
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
		/// </summary>
		UIEvent,
		/// <summary>
		/// </summary>
		EditorForm,
	}

	/// <summary>
	/// Represents a given class.
	/// </summary>
	public struct CryScript
	{
		public CryScript(Type type)
			: this()
		{
			ScriptType = type;
		}

		public Type ScriptType { get; private set; }

		/// <summary>
		/// Stores all instances of this class.
		/// </summary>
		public Collection<CryScriptInstance> ScriptInstances { get; internal set; }

		#region Operators
		public static bool operator ==(CryScript script1, CryScript script2)
		{
			return script1.ScriptType == script2.ScriptType;
		}

		public static bool operator !=(CryScript script1, CryScript script2)
		{
			return script1.ScriptType != script2.ScriptType;
		}

		public override bool Equals(object obj)
		{
			if (obj is CryScript)
				return (CryScript)obj == this;

			return false;
		}

		public override int GetHashCode()
		{
			return ScriptType.GetHashCode();
		}

		#endregion
	}

    [Serializable]
	public class ScriptCompilationException : Exception
	{
		public ScriptCompilationException(string errorMessage)
			: base(errorMessage) { }

		public ScriptCompilationException(string errorMessage, Exception innerEx)
			: base(errorMessage, innerEx) { }

		public override string Message { get { return base.Message.ToString(); } }
	}

    [Serializable]
	class ScriptNotFoundException : Exception
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