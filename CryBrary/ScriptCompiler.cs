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

namespace CryEngine.Initialization
{
	public class ScriptCompiler
	{
		public ScriptCompiler()
		{
			FlowNodes = new List<string>();

			assemblyReferenceHandler = new AssemblyReferenceHandler();
		}

		public bool Initialize()
		{
			LastScriptId = 0;

			var foundTypes = new List<Type>();
			foundTypes.AddRange(LoadLibrariesInFolder(Path.Combine(PathUtils.GetScriptsFolder(), "Plugins")));

			var compilationParams = new CompilationParameters();

			compilationParams.Folders = new string[] 
			{ 
				PathUtils.GetScriptFolder(ScriptType.Entity),
				PathUtils.GetScriptFolder(ScriptType.GameRules),
				PathUtils.GetScriptFolder(ScriptType.FlowNode),
				PathUtils.GetScriptFolder(ScriptType.UIEvent),

				PathUtils.GetScriptsFolder()
			};

			try
			{
				foundTypes.AddRange(CompileScripts(ref compilationParams));
			}
			catch(ScriptCompilationException ex)
			{
				Debug.LogException(ex);

				return false;
			}

			ProcessTypes(foundTypes);
			foundTypes.Clear();
			foundTypes = null;

			return true;
		}

		public void PostInit()
		{
			// These have to be registered later on due to the flow system being initialized late.
			//foreach(var node in FlowNodes)
			//FlowNode.RegisterNode(node);
		}

		/// <summary>
		/// Instantiates a script using its name and interface.
		/// </summary>
		/// <param name="scriptName"></param>
		/// <param name="constructorParams"></param>
		/// <returns>New instance scriptId or -1 if instantiation failed.</returns>
		[EditorBrowsable(EditorBrowsableState.Never)]
		public CryScriptInstance InstantiateScript(string scriptName, object[] constructorParams = null)
		{
			if(scriptName.Length < 1)
				throw new ArgumentException("Empty script name passed to InstantiateClass");

			var script = default(CryScript);
			var scriptIndex = 0;
			for(; scriptIndex < CompiledScripts.Length; scriptIndex++)
			{
				var foundScript = CompiledScripts[scriptIndex];

				if(foundScript.ScriptName.Equals(scriptName) || (foundScript.ScriptName.Contains(scriptName) && foundScript.ScriptType.Name.Equals(scriptName)))
				{
					script = foundScript;
					break;
				}
			}

			if(script == default(CryScript))
				throw new ScriptNotFoundException(string.Format("Compiled script {0} could not be found.", scriptName));

			return AddScriptInstance(System.Activator.CreateInstance(script.ScriptType, constructorParams) as CryScriptInstance, script, scriptIndex);
		}

		/// <summary>
		/// Adds an script instance to the script collection and returns its new id.
		/// </summary>
		/// <param name="instance"></param>
		public static CryScriptInstance AddScriptInstance(CryScriptInstance instance)
		{
			if(instance == null)
				return null;

			int scriptIndex;
			var script = GetScriptByType(instance.GetType(), out scriptIndex);

			return AddScriptInstance(instance, script, scriptIndex);
		}

		static CryScriptInstance AddScriptInstance(CryScriptInstance instance, CryScript script, int scriptIndex)
		{
			if(instance == null)
				return null;

			if(script.ScriptInstances == null)
				script.ScriptInstances = new List<CryScriptInstance>();
			else if(script.ScriptInstances.Contains(instance))
				return null;

			LastScriptId++;

			instance.ScriptId = LastScriptId;
			script.ScriptInstances.Add(instance);

			CompiledScripts[scriptIndex] = script;

			return script.ScriptInstances.Last();
		}

		public void RemoveInstance(int scriptId)
		{
			RemoveInstance(scriptId, null);
		}

		/// <summary>
		/// Locates and destructs the script with the assigned scriptId.
		/// </summary>
		/// <param name="scriptId"></param>
		public void RemoveInstance(int scriptId, Type scriptType)
		{
			if(scriptType != null)
			{
				int scriptIndex;
				var script = GetScriptByType(scriptType, out scriptIndex);

				RemoveInstanceFromScriptById(ref script, scriptId);

				CompiledScripts[scriptIndex] = script;
			}
			else
			{
				for(int i = 0; i < CompiledScripts.Length; i++)
				{
					var script = CompiledScripts[i];

					RemoveInstanceFromScriptById(ref script, scriptId);

					CompiledScripts[i] = script;
				}
			}
		}

		void RemoveInstanceFromScriptById(ref CryScript script, int scriptId)
		{
			if(script.ScriptInstances != null && script.ScriptInstances.Count > 0)
			{
				var scriptInstance = script.ScriptInstances.FirstOrDefault(x => x.ScriptId == scriptId);
				if(scriptInstance == InvalidScriptInstance)
					return;

				var instanceIndex = script.ScriptInstances.IndexOf(scriptInstance);
				if(instanceIndex == -1)
				{
					Debug.LogAlways("Failed to remove script with id {0}; instance was not found.", scriptId);
					return;
				}

				script.ScriptInstances.RemoveAt(instanceIndex);
			}
		}

		internal static CryScript GetScriptByType(Type type, out int scriptIndex)
		{
			var script = default(CryScript);
			scriptIndex = 0;
			for(; scriptIndex < CompiledScripts.Length; scriptIndex++)
			{
				var foundScript = CompiledScripts[scriptIndex];

				if(foundScript.ScriptType == type)
				{
					script = foundScript;
					break;
				}
			}

			if(script == default(CryScript))
				throw new ScriptNotFoundException(string.Format("Failed to find script by name {0}", type.Name));

			return script;
		}

		public static CryScriptInstance GetScriptInstanceById(int id)
		{
			CryScriptInstance scriptInstance = null;
			foreach(var script in CompiledScripts)
			{
				if(script.ScriptInstances != null && script.ScriptInstances.Count > 0)
				{
					scriptInstance = script.ScriptInstances.FirstOrDefault(instance => instance.ScriptId == id);

					if(scriptInstance != InvalidScriptInstance)
						return scriptInstance;
				}
			}

			return null;
		}

		public int GetEntityScriptId(EntityId entityId, System.Type scriptType = null)
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
		public void OnUpdate(float frameTime)
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
		}

		/// <summary>
		/// This function will automatically scan for C# dll (*.dll) files and load the types contained within them.
		/// </summary>
		public IEnumerable<Type> LoadLibrariesInFolder(string directory)
		{
			if(Directory.Exists(directory))
			{
				var plugins = Directory.GetFiles(directory, "*.dll", SearchOption.AllDirectories);

				if(plugins != null && plugins.Length != 0)
				{
					var typeCollection = new List<Type>();

					foreach(var plugin in plugins)
					{
						try
						{
							var newPath = Path.Combine(Path.GetTempPath(), Path.GetFileName(plugin));

							File.Copy(plugin, newPath, true);
#if !RELEASE
							GenerateDebugDatabaseForAssembly(plugin);

							var mdbFile = plugin + ".mdb";
							if(File.Exists(mdbFile)) // success
								File.Copy(mdbFile, Path.Combine(Path.GetTempPath(), Path.GetFileName(mdbFile)), true);
#endif
							foreach(var type in LoadAssembly(Assembly.LoadFrom(newPath)))
								typeCollection.Add(type);
						}
						//This exception tells us that the assembly isn't a valid .NET assembly for whatever reason
						catch(BadImageFormatException)
						{
							Debug.LogAlways("Plugin loading failed for {0}; dll is not valid.", plugin);
						}
					}

					return typeCollection;
				}
			}
			else
				Debug.LogAlways("Skipping load of plugins in {0}, directory does not exist.", directory);

			return Enumerable.Empty<Type>();
		}

		/// <summary>
		/// Compiles the scripts and compiles them into an assembly.
		/// </summary>
		/// <param name="compilationParameters"></param>
		/// <returns></returns>
		/// <exception cref="System.IO.DirectoryNotFoundException">Thrown when one or more script folders could not be located.</exception>
		/// <exception cref="System.ArgumentNullException">Thrown if the compiled assembly could not be loaded.</exception>
		/// <exception cref="CryEngine.Initialization.ScriptCompilationException">Thrown if one or more compilation errors occur.</exception>
		public IEnumerable<Type> CompileScripts(ref CompilationParameters compilationParameters)
		{
			if(compilationParameters == null)
				throw new ArgumentException("CompilationParameters was null");

			if(compilationParameters.Folders == null)
				throw new ArgumentException(message: "Supplied Folders array in CompilationParameters argument was null");
			else if(compilationParameters.Folders.Length < 1)
				throw new ArgumentException(message: "Supplied Folders array in CompilationParameters did not contain any strings");

			var scripts = new List<string>();
			foreach(var directory in compilationParameters.Folders)
			{
				if(Directory.Exists(directory))
				{
					foreach(var script in Directory.GetFiles(directory, "*.cs", SearchOption.AllDirectories))
					{
						if(!scripts.Contains(script))
							scripts.Add(script);
					}
				}
				else
					Debug.LogAlways("Skipping compilation of scripts in {0}; directory not found", directory);
			}

			if(scripts.Count <= 0)
				return Enumerable.Empty<Type>();

			var compilerParameters = new CompilerParameters();

			compilerParameters.GenerateExecutable = false;

			bool includeDebugInfo = true;

#if RELEASE
			if(!compilationParameters.ForceDebugInformation)
				includeDebugInfo = false;
#endif

			// Necessary for stack trace line numbers etc
			compilerParameters.IncludeDebugInformation = includeDebugInfo;
			compilerParameters.GenerateInMemory = !includeDebugInfo;

			compilerParameters.ReferencedAssemblies.AddRange(assemblyReferenceHandler.GetRequiredAssembliesForScriptFiles(scripts));

			CompilerResults results;
			using(var provider = GetCodeProvider(compilationParameters.Language))
				results = provider.CompileAssemblyFromFile(compilerParameters, scripts.ToArray());

			compilerParameters = null;
			compilationParameters.Results = results;

			if(results.CompiledAssembly != null) // success
				return LoadAssembly(results.CompiledAssembly, compilationParameters.ScriptTypes);
			else if(results.Errors.HasErrors)
			{
				string compilationError = string.Format("Compilation failed; {0} errors: ", results.Errors.Count);

				foreach(CompilerError error in results.Errors)
				{
					compilationError += Environment.NewLine;

					if(!error.ErrorText.Contains("(Location of the symbol related to previous error)"))
						compilationError += string.Format("{0}({1},{2}): {3} {4}: {5}", error.FileName, error.Line, error.Column, error.IsWarning ? "warning" : "error", error.ErrorNumber, error.ErrorText);
					else
						compilationError += "	" + error.ErrorText;
				}
				throw new ScriptCompilationException(compilationError);
			}

			throw new ArgumentNullException(paramName: "Tried loading a NULL assembly");
		}

		public CodeDomProvider GetCodeProvider(ScriptLanguage language)
		{
			switch(language)
			{
				case ScriptLanguage.VisualBasic:
					return CodeDomProvider.CreateProvider("VisualBasic");
				case ScriptLanguage.JScript:
					return CodeDomProvider.CreateProvider("JScript");
				case ScriptLanguage.CSharp:
					return CodeDomProvider.CreateProvider("CSharp");
			}

			return null;
		}

		/// <summary>
		/// Loads an C# assembly and adds all found types to ScriptCompiler.CompiledScripts
		/// </summary>
		public IEnumerable<Type> LoadAssembly(Assembly assembly, ScriptType[] allowedTypes = null)
		{
			var types = new List<Type>();

			Parallel.ForEach(assembly.GetTypes(), type =>
			{
				// TODO: Re-implement allowedTypes
				if(!type.ContainsAttribute<ExcludeFromCompilationAttribute>())
					types.Add(type);
			});

			return types;
		}

		public ScriptType GetScriptType(Type type)
		{
			if(type.Implements(typeof(GameRules)))
				return ScriptType.GameRules;
			else if(type.Implements(typeof(Actor)))
				return ScriptType.Actor;
			else if(type.Implements(typeof(Entity)))
				return ScriptType.Entity;
			else if(type.Implements(typeof(FlowNode)))
				return ScriptType.FlowNode;

			return ScriptType.Unknown;
		}

		public bool IsScriptType(Type type, ScriptType scriptType)
		{
			return GetScriptType(type) == scriptType || scriptType == ScriptType.Unknown; // Return true for unknown scripts as well
		}

		/// <summary>
		/// Processes a type and adds all found types to ScriptCompiler.CompiledScripts
		/// </summary>
		/// <param name="type"></param>
		public void ProcessTypes(IEnumerable<Type> types)
		{
			Type[] specialTypes = { typeof(NativeEntity) };

			CompiledScripts = new CryScript[types.Count() + specialTypes.Length];

			for(int i = 0; i < types.Count(); i++)
			{
				var type = types.ElementAt(i);
				var script = new CryScript(type);

				if(type.IsAbstract)
				{
					// Add anyway, to fix serialization bug caused by child types within abstract entities.
					CompiledScripts[i] = script;
					continue;
				}

				if(type.Implements(typeof(GameRules)))
				{
					string gamemodeName = null;

					GameRulesAttribute gamemodeAttribute;
					if(type.TryGetAttribute<GameRulesAttribute>(out gamemodeAttribute))
					{
						if(!string.IsNullOrEmpty(gamemodeAttribute.Name))
							gamemodeName = gamemodeAttribute.Name;

						if(gamemodeAttribute.Default)
							GameRules._SetDefaultGameMode(gamemodeName);
					}

					GameRules._RegisterGameMode(gamemodeName ?? script.ScriptName);
				}
				else if(type.Implements(typeof(Actor)))
					Actor._RegisterActorClass(script.ScriptName, false);
				else if(type.Implements(typeof(Entity)))
					LoadEntity(ref script);
				else if(type.Implements(typeof(FlowNode)))
					LoadFlowNode(ref script);

				CompiledScripts[i] = script;
			}

			for(int i = 0; i < specialTypes.Length; i++)
				CompiledScripts[types.Count() + i] = new CryScript(specialTypes[i]);
		}

		private void LoadEntity(ref CryScript script)
		{
			Entity.RegisterEntityClass(Entity.GetEntityConfig(script.ScriptType));

			LoadFlowNode(ref script, true);
		}

		private void LoadFlowNode(ref CryScript script, bool entityNode = false)
		{
			string category = null;
			var nodeName = script.ScriptName;

			if(!entityNode)
			{
				category = script.ScriptType.Namespace;

				FlowNodeAttribute nodeInfo;
				if(script.ScriptType.TryGetAttribute<FlowNodeAttribute>(out nodeInfo))
				{
					if(nodeInfo.UICategory != null && nodeInfo.UICategory.Length > 0)
						category = nodeInfo.UICategory;

					if(nodeInfo.Name != null && nodeInfo.Name.Length > 0)
						nodeName = nodeInfo.Name;
				}

				script.ScriptName = category + ":" + nodeName;
			}
			else
				category = "entity";

			FlowNodes.Add(category + ":" + nodeName);
		}

		public void GenerateDebugDatabaseForAssembly(string assemblyPath)
		{
			if(File.Exists(Path.ChangeExtension(assemblyPath, "pdb")))
			{
				var assembly = Assembly.LoadFrom(Path.Combine(PathUtils.GetEngineFolder(), "Mono", "bin", "pdb2mdb.dll"));
				var driver = assembly.GetType("Driver");
				var convertMethod = driver.GetMethod("Convert", BindingFlags.Static | BindingFlags.Public);

				object[] args = { assemblyPath };
				convertMethod.Invoke(null, args);
			}
		}

		/// <summary>
		/// Last assigned ScriptId, next = + 1
		/// </summary>
		public static int LastScriptId;

		internal List<string> FlowNodes;
		AssemblyReferenceHandler assemblyReferenceHandler;

		/// <summary>
		/// Avoid creating a new empty CryScriptInstance each time we need to check
		/// </summary>
		static CryScriptInstance InvalidScriptInstance = default(CryScriptInstance);

		public static CryScript[] CompiledScripts { get; internal set; }

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
			/// Determines what types of scripts should be compiled.
			/// Accepts all scripts by default.
			/// </summary>
			public ScriptType[] ScriptTypes { get; set; }

			/// <summary>
			/// Forces generation of debug information, even in release mode.
			/// </summary>
			public bool ForceDebugInformation { get; set; }
		}
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
			ScriptName = type.Name;
		}

		public Type ScriptType { get; private set; }
		/// <summary>
		/// The script's name, not always type name!
		/// </summary>
		public string ScriptName { get; set; }

		/// <summary>
		/// Stores all instances of this class.
		/// </summary>
		public List<CryScriptInstance> ScriptInstances { get; internal set; }

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
			if(obj is CryScript)
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