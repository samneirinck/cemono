using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Reflection;
using CryEngine.Extensions;

namespace CryEngine.Initialization
{
	class ScriptManager
	{
		public ScriptManager()
		{
			if(CompiledScripts == null)
			{
				CompiledScripts = new Dictionary<ScriptType, List<CryScript>>();

				foreach(ScriptType scriptType in Enum.GetValues(typeof(ScriptType)))
					CompiledScripts.Add(scriptType, new List<CryScript>());
			}

			if(!Directory.Exists(PathUtils.TempFolder))
				Directory.CreateDirectory(PathUtils.TempFolder);
			else
			{
				try
				{
					foreach(var file in Directory.GetFiles(PathUtils.TempFolder))
						File.Delete(file);
				}
				catch(UnauthorizedAccessException) { }
			}

			FlowNodes = new List<string>();

			Type[] specialTypes = { typeof(NativeEntity), typeof(NativeActor) };
			foreach(var type in specialTypes)
				CreateScript(type);
		}

		bool LoadPlugins()
		{
			LoadLibrariesInFolder(Path.Combine(PathUtils.ScriptsFolder, "Plugins"));

			return true;
		}

		public void PostInit()
		{
			// These have to be registered later on due to the flow system being initialized late.
			foreach(var node in FlowNodes)
				FlowNode.Register(node);
		}

		/// <summary>
		/// This function will automatically scan for C# dll (*.dll) files and load the types contained within them.
		/// </summary>
		public void LoadLibrariesInFolder(string directory)
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
							LoadAssembly(plugin);
						}
						//This exception tells us that the assembly isn't a valid .NET assembly for whatever reason
						catch(BadImageFormatException)
						{
							Debug.LogAlways("Plugin loading failed for {0}; dll is not valid.", plugin);
						}
					}
				}
			}
			else
				Debug.LogAlways("Skipping load of plugins in {0}, directory does not exist.", directory);
		}

		/// <summary>
		/// Processes a C# assembly and adds all found types to ScriptCompiler.CompiledScripts
		/// </summary>
		public void ProcessAssembly(Assembly assembly)
		{
			foreach(var type in assembly.GetTypes())
				ProcessType(type);
		}

		/// <summary>
		/// Loads a C# assembly by location, creates a shadow-copy and generates debug database (mdb).
		/// </summary>
		/// <param name="assemblyPath"></param>
		public void LoadAssembly(string assemblyPath)
		{
			var newPath = Path.Combine(PathUtils.TempFolder, Path.GetFileName(assemblyPath));
			
			TryCopyFile(assemblyPath, ref newPath, true);

#if !RELEASE
			GenerateDebugDatabaseForAssembly(assemblyPath);

			var mdbFile = assemblyPath + ".mdb";
			if(File.Exists(mdbFile)) // success
			{
				var newMdbPath = Path.Combine(PathUtils.TempFolder, Path.GetFileName(mdbFile));
				TryCopyFile(mdbFile, ref newMdbPath);
			}
#endif

			ProcessAssembly(Assembly.LoadFrom(newPath));
		}

		void TryCopyFile(string currentPath, ref string newPath, bool overwrite = true)
		{
			if(!File.Exists(newPath))
				File.Copy(currentPath, newPath, overwrite);
			else
			{
				try
				{
					File.Copy(currentPath, newPath, overwrite);
				}
				catch(Exception ex)
				{
					if(ex is UnauthorizedAccessException || ex is IOException)
					{
						newPath = Path.ChangeExtension(newPath, "_" + Path.GetExtension(newPath));
						TryCopyFile(currentPath, ref newPath);
					}
					else
						throw;
				}
			}
		}

		/// <summary>
		/// Adds the type to CompiledScripts
		/// </summary>
		/// <param name="type"></param>
		internal void ProcessType(Type type)
		{
			var script = new CryScript(type);

			if(!type.IsAbstract && !type.ContainsAttribute<ExcludeFromCompilationAttribute>())
			{
				switch(script.ScriptType)
				{
					case ScriptType.Actor:
						LoadActor(ref script);
						break;
					case ScriptType.Entity:
						LoadEntity(ref script);
						break;
					case ScriptType.FlowNode:
						LoadFlowNode(ref script);
						break;
					case ScriptType.GameRules:
						LoadGameRules(ref script);
						break;
					case ScriptType.ScriptCompiler:
						{
							Debug.LogAlways("		Compiling scripts using {0}...", type.Name);
							var compiler = Activator.CreateInstance(type) as ScriptCompiler;
							ProcessAssembly(compiler.Compile());
						}
						break;
				}
			}

			CompiledScripts[script.ScriptType].Add(script);
		}

		void LoadActor(ref CryScript script)
		{
			bool registerActorClass = true;
			bool isAI = false;

			ActorAttribute attr;
			if(script.Type.TryGetAttribute<ActorAttribute>(out attr))
			{
				registerActorClass = attr.useMonoActor;
				isAI = attr.isAI;
			}

			if(registerActorClass)
				Actor._RegisterActorClass(script.ScriptName, isAI);
		}

		void LoadEntity(ref CryScript script)
		{
			//LoadFlowNode(ref script, true);

			Entity.Methods.RegisterClass(Entity.GetEntityConfig(script.Type));
		}

		void LoadFlowNode(ref CryScript script, bool entityNode = false)
		{
			bool containsNodePorts = false;
			foreach(var member in script.Type.GetMembers())
			{
				if(member.ContainsAttribute<PortAttribute>())
				{
					containsNodePorts = true;
					break;
				}
			}

			if(!containsNodePorts)
				return;

			string category = null;
			var nodeName = script.ScriptName;

			if(!entityNode)
			{
				category = script.Type.Namespace;

				FlowNodeAttribute nodeInfo;
				if(script.Type.TryGetAttribute<FlowNodeAttribute>(out nodeInfo))
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

		void LoadGameRules(ref CryScript script)
		{
			string gamemodeName = null;

			GameRulesAttribute gamemodeAttribute;
			if(script.Type.TryGetAttribute<GameRulesAttribute>(out gamemodeAttribute))
			{
				if(!string.IsNullOrEmpty(gamemodeAttribute.Name))
					gamemodeName = gamemodeAttribute.Name;

				if(gamemodeAttribute.Default)
					GameRules._SetDefaultGameMode(gamemodeName);
			}

			GameRules._RegisterGameMode(gamemodeName ?? script.ScriptName);
		}

		public void GenerateDebugDatabaseForAssembly(string assemblyPath)
		{
			if(File.Exists(Path.ChangeExtension(assemblyPath, "pdb")))
			{
				var assembly = Assembly.LoadFrom(Path.Combine(PathUtils.EngineFolder, "Mono", "bin", "pdb2mdb.dll"));
				var driver = assembly.GetType("Driver");
				var convertMethod = driver.GetMethod("Convert", BindingFlags.Static | BindingFlags.Public);

				object[] args = { assemblyPath };
				convertMethod.Invoke(null, args);
			}
		}

		List<string> FlowNodes { get; set; }

		#region Statics
		/// <summary>
		/// Called once per frame.
		/// </summary>
		public static void OnUpdate(float frameTime)
		{
			Time.DeltaTime = frameTime;

			foreach(var scriptList in CompiledScripts)
			{
				if(scriptList.Key == ScriptType.Unknown)
					continue;

				scriptList.Value.ForEach(script => 
					{
						if(script.ScriptInstances != null)
						{
							script.ScriptInstances.ForEach(instance =>
								{
									if(instance.ReceiveUpdates)
										instance.OnUpdate();
								});
						}
					});
			}
		}

		static CryScript CreateScript(Type type)
		{
			var script = new CryScript(type);
			ScriptManager.CompiledScripts[script.ScriptType].Add(script);

			return script;
		}

		public static IEnumerable<CryScript> GetScriptList(ScriptType scriptType)
		{
			if(scriptType == ScriptType.Unknown)
			{
				var newList = new List<CryScript>();

				foreach(var scriptList in CompiledScripts)
				{
					if(scriptList.Value != null)
						newList.AddRange(scriptList.Value);
				}

				return newList;
			}

			return CompiledScripts[scriptType];
		}

		/// <summary>
		/// Instantiates a script using its name and interface.
		/// </summary>
		/// <param name="scriptName"></param>
		/// <param name="constructorParams"></param>
		/// <returns>New instance scriptId or -1 if instantiation failed.</returns>
		public static object InstantiateScript(string scriptName, ScriptType scriptType = ScriptType.Unknown, object[] constructorParams = null)
		{
			if(scriptName == null)
				throw new ArgumentNullException("scriptName");
			else if(scriptName.Length < 1)
				throw new ArgumentException("Empty script name passed to InstantiateClass");

			var script = GetScriptList(scriptType).FirstOrDefault(x => x.ScriptName.Equals(scriptName) || (x.ScriptName.Contains(scriptName) && x.Type.Name.Equals(scriptName)));

			if(script == default(CryScript))
				throw new ScriptNotFoundException(string.Format("Script {0} of ScriptType {1} could not be found.", scriptName, scriptType));

			var scriptInstance = Activator.CreateInstance(script.Type, constructorParams);
			AddScriptInstance(scriptInstance as CryScriptInstance, script);

			if(scriptType == ScriptType.GameRules)
				GameRules.Current = (GameRules)scriptInstance;

			return scriptInstance;
		}

		public static void RemoveInstance(int scriptId, ScriptType scriptType = ScriptType.Unknown)
		{
			RemoveInstance(scriptId, scriptType, null);
		}

		/// <summary>
		/// Locates and destructs the script with the assigned scriptId.
		/// </summary>
		/// <param name="scriptId"></param>
		public static void RemoveInstance(int scriptId, ScriptType scriptType = ScriptType.Unknown, Type type = null)
		{
			if(type != null)
			{
				var script = GetScriptList(scriptType).First(x => x.Type == type);
				if(script == null)
					throw new TypeLoadException(string.Format("Failed to locate type {0}", type.Name));

				if(script.ScriptInstances != null && script.ScriptInstances.RemoveAll(x => x.ScriptId == scriptId) > 0)
				{
					int index = CompiledScripts[script.ScriptType].FindIndex(x => x.Type == script.Type);
					CompiledScripts[script.ScriptType].Insert(index, script);
				}
			}
			else
			{
				foreach(var script in GetScriptList(scriptType))
				{
					if(script.ScriptInstances != null && script.ScriptInstances.RemoveAll(x => x.ScriptId == scriptId) > 0)
					{
						int index = CompiledScripts[script.ScriptType].FindIndex(x => x.Type == script.Type);
						CompiledScripts[script.ScriptType].Insert(index, script);
					}
				}
			}


		}

		public static int GetEntityScriptId(EntityId entityId, ScriptType scriptType = ScriptType.Unknown, Type type = null)
		{
			var scripts = GetScriptList(scriptType).Where(script => (type != null ? script.Type.Implements(type) : true) && script.ScriptInstances != null);

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

		public static CryScriptInstance GetScriptInstanceById(int id, ScriptType scriptType = ScriptType.Unknown)
		{
			CryScriptInstance scriptInstance = null;
			foreach(var script in GetScriptList(scriptType))
			{
				if(script.ScriptInstances != null)
				{
					scriptInstance = script.ScriptInstances.First(instance => instance.ScriptId == id);

					if(scriptInstance != null)
						return scriptInstance;
				}
			}

			return null;
		}

		internal static T FindScriptInstance<T>(Predicate<T> match, ScriptType scriptType = ScriptType.Unknown) where T : CryScriptInstance
		{
			T result;
			foreach(var script in GetScriptList(scriptType))
			{
				if((script.Type.Equals(typeof(T)) || script.Type.Implements(typeof(T))) && script.ScriptInstances != null)
				{
					result = script.ScriptInstances.Find(x => match(x as T)) as T;
					if(result != null)
						return result;
				}
			}

			return null;
		}

		public static void AddScriptInstance(CryScriptInstance instance, ScriptType scriptType)
		{
			if(instance == null)
				throw new ArgumentNullException("instance");

			var script = GetScriptList(scriptType).First(x => x.Type == instance.GetType());
			if(script == null)
				script = CreateScript(instance.GetType());

			AddScriptInstance(instance, script);
		}

		/// <summary>
		/// Adds an script instance to the script collection and returns its new id.
		/// </summary>
		/// <param name="instance"></param>
		public static void AddScriptInstance(CryScriptInstance instance, CryScript script)
		{
			if(instance == null)
				throw new ArgumentNullException("instance");

			instance.ScriptId = LastScriptId++;

			if(script.ScriptInstances == null)
				script.ScriptInstances = new List<CryScriptInstance>();

			script.ScriptInstances.Add(instance);

			int index = CompiledScripts[script.ScriptType].FindIndex(x => x.Type == script.Type);
			CompiledScripts[script.ScriptType].Insert(index, script);
		}

		/// <summary>
		/// Last assigned ScriptId, next = + 1
		/// </summary>
		public static int LastScriptId = 0;

		internal static Dictionary<ScriptType, List<CryScript>> CompiledScripts { get; set; }
		#endregion
	}

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