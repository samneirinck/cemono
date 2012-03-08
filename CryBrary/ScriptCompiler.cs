using System;
using System.Linq;
using System.IO;

using System.Collections.ObjectModel;
using System.Collections.Generic;

using System.CodeDom.Compiler;
using System.Reflection;

using CryEngine.Extensions;
using CryEngine.Utils;

using System.ComponentModel;

using System.Threading.Tasks;

namespace CryEngine
{
	public static partial class ScriptCompiler
	{
		public static bool Initialize()
		{

			CompiledScripts = new List<CryScript>();
			FlowNodes = new Collection<StoredNode>();

            assemblyReferenceHandler = new AssemblyReferenceHandler();

			NextScriptId = 0;

			//GenerateScriptbindAssembly(scriptBinds.ToArray());

			LoadPrecompiledAssemblies();

			var compilationParams = new CompilationParameters();

			compilationParams.Folders = new string[] 
			{ 
				PathUtils.GetScriptFolder(ScriptType.Entity),
				PathUtils.GetScriptFolder(ScriptType.GameRules),
				PathUtils.GetScriptFolder(ScriptType.FlowNode) 
			};

			try
			{
				CompileScripts(ref compilationParams);
			}
			catch(Exception ex)
			{
				if(compilationParams.Results.Errors.HasErrors)
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

			AddScriptInstance(System.Activator.CreateInstance(CompiledScripts[index].ClassType, constructorParams) as CryScriptInstance, index);

			return CompiledScripts[index].ScriptInstances.Last();
		}

		/// <summary>
		/// Adds an script instance to the script collection and returns its new id.
		/// </summary>
		/// <param name="instance"></param>
		/// <param name="compiledScriptsIndex">Index of the CryScript object residing in ScriptCompiler.CompiledScripts</param>
		public static int AddScriptInstance(CryScriptInstance instance, int scriptIndex = -1)
		{
			if(scriptIndex == -1)
				scriptIndex = CompiledScripts.FindIndex(x => x.ClassName == instance.GetType().Name);

			if(scriptIndex != -1)
			{
				var script = CompiledScripts.ElementAt(scriptIndex);

				if(script.ScriptType == ScriptType.GameRules)
					script.ScriptInstances = null;

				if(script.ScriptInstances == null)
					script.ScriptInstances = new List<CryScriptInstance>();
				else if(script.ScriptInstances.Contains(instance))
					return -1;

				NextScriptId++;

				instance.ScriptId = NextScriptId;
				script.ScriptInstances.Add(instance);

				CompiledScripts[scriptIndex] = script;

				return NextScriptId;
			}

			Debug.LogAlways("Failed to add object of type {0} to list of compiled scripts", instance.GetType().Name);
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
				int i = CompiledScripts.FindIndex(x => x.ClassName.Equals(scriptName));
				if(i == -1)
					return;

				CryScript script = CompiledScripts[i];

				RemoveInstanceFromScriptById(ref script, scriptId);

				CompiledScripts[i] = script;
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
				var scriptInstance = script.ScriptInstances.First(x => x.ScriptId == scriptId);
				if(scriptInstance == null)
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
		/// Called once per frame.
		/// </summary>
		public static void OnUpdate(float frameTime)
		{
			Time.DeltaTime = frameTime;

			foreach(var script in CompiledScripts)
			{
				if(script.ScriptInstances != null)
					script.ScriptInstances.ForEach(i =>
					{
						if(i.ReceiveUpdates)
							i.OnUpdate();
					});
			}

			EntitySystem.OnUpdate();
		}

		/// <summary>
		/// This function will automatically scan for C# dll (*.dll) files and load the types contained within them.
		/// </summary>
		public static void LoadLibrariesInFolder(string directory)
		{
			if (!Directory.Exists(directory))
			{
				Debug.LogAlways("Libraries failed to load; Folder {0} does not exist.", directory);
				return;
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

				if(compiledScripts.Count < 1)
					return;

				CompiledScripts.AddRange(compiledScripts);
			}

			return;
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
		/// <param name="compilationParams"></param>
		/// <returns></returns>
		/// <exception cref="System.IO.DirectoryNotFoundException">Thrown when one or more script folders could not be located.</exception>
		/// <exception cref="System.ArgumentNullException">Thrown if the compiled assembly could not be loaded.</exception>
		/// <exception cref="CryEngine.ScriptCompilationException">Thrown if one or more compilation errors occur.</exception>
		public static void CompileScripts(ref CompilationParameters compilationParams)
		{
			if (compilationParams.Folders == null || compilationParams.Folders.Length < 1)
				return;

			var scripts = new List<string>();
			foreach(var directory in compilationParams.Folders)
			{
				if (Directory.Exists(directory))
					scripts.AddRange(Directory.GetFiles(directory, "*.cs", SearchOption.AllDirectories));
				else
					throw new DirectoryNotFoundException(string.Format("Could not compile scripts in {0}; directory not found", directory));
			}

			if(scripts.Count <= 0)
				return;

			CodeDomProvider provider = null;
			switch(compilationParams.Language)
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

            compilerParameters.ReferencedAssemblies.AddRange(assemblyReferenceHandler.ReferencedAssemblies.ToArray());

			CompilerResults results = provider.CompileAssemblyFromFile(compilerParameters, scripts.ToArray());

			provider.Dispose();
			provider = null;
			compilerParameters = null;

			compilationParams.Results = results;

			if(results.CompiledAssembly != null) // success
			{
				var compiledScripts = LoadAssembly(results.CompiledAssembly);
				if(compiledScripts != null && compiledScripts.Count() > 0)
					CompiledScripts.AddRange(compiledScripts);

			}
			else if(results.Errors.HasErrors)
			{
				string compilationError = string.Format("Compilation failed; {0} errors: ", results.Errors.Count);

				foreach(CompilerError error in results.Errors)
					compilationError += error.ErrorText;

				throw new ScriptCompilationException(compilationError);
			}
			else
				throw new ArgumentNullException("Tried loading a NULL assembly");
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

			entity = null;

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
		public List<CryScriptInstance> ScriptInstances { get; internal set; }

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

	public class ScriptCompilationException : Exception
	{
		public ScriptCompilationException(string errorMessage)
			: base(errorMessage) { }

		public ScriptCompilationException(string errorMessage, Exception innerEx)
			: base(errorMessage, innerEx) { }

		public string Message { get { return base.Message.ToString(); } }
	}
}