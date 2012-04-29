using System;
using System.Linq;
using System.IO;

using System.Collections.Generic;

using System.Reflection;

using CryEngine.Extensions;

using System.ComponentModel;

using System.Threading.Tasks;

namespace CryEngine.Initialization
{
	class ScriptManager
	{
		internal ScriptManager()
		{
			FlowNodes = new List<string>();
		}

		public bool Initialize()
		{
			Type[] specialTypes = { typeof(NativeEntity) };
			foreach(var type in specialTypes)
				CompiledScripts.Add(new CryScript(type));

			LoadLibrariesInFolder(Path.Combine(PathUtils.GetScriptsFolder(), "Plugins"));

			return true;
		}

		public void PostInit()
		{
			// These have to be registered later on due to the flow system being initialized late.
			foreach(var node in FlowNodes)
				FlowNode.Register(node);
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
			for(; scriptIndex < CompiledScripts.Count; scriptIndex++)
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
				for(int i = 0; i < CompiledScripts.Count; i++)
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
			for(; scriptIndex < CompiledScripts.Count; scriptIndex++)
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
							var newPath = Path.Combine(Path.GetTempPath(), Path.GetFileName(plugin));

							File.Copy(plugin, newPath, true);
#if !RELEASE
							GenerateDebugDatabaseForAssembly(plugin);

							var mdbFile = plugin + ".mdb";
							if(File.Exists(mdbFile)) // success
								File.Copy(mdbFile, Path.Combine(Path.GetTempPath(), Path.GetFileName(mdbFile)), true);
#endif

							LoadAssembly(Assembly.LoadFrom(newPath));
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
		/// Loads an C# assembly and adds all found types to ScriptCompiler.CompiledScripts
		/// </summary>
		public void LoadAssembly(Assembly assembly, ScriptType[] allowedTypes = null)
		{
			var types = new List<Type>();

			Parallel.ForEach(assembly.GetTypes(), type =>
			{
				// TODO: Re-implement allowedTypes
				if(!type.ContainsAttribute<ExcludeFromCompilationAttribute>())
					types.Add(type);
			});

			ProcessTypes(types);
		}

		/// <summary>
		/// Processes a type and adds all found types to ScriptCompiler.CompiledScripts
		/// </summary>
		/// <param name="types"></param>
		void ProcessTypes(IEnumerable<Type> types)
		{
			// Load custom script compilers
			/*
			foreach(var type in types.Where(type => type.Implements(typeof(IScriptCompiler))))
			{
				var compiler = Activator.CreateInstance(type) as IScriptCompiler;
				Debug.LogAlways("	Running custom compiler: {0}", compiler.GetType().Name);
				typeList.AddRange(LoadAssembly(compiler.Compile()));
			}*/

			foreach(var type in types)
			{
				var script = new CryScript(type);

				if(!type.IsAbstract)
				{
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
					else if(type.Implements(typeof(FlowNode)))
					{
						LoadFlowNode(ref script);

						if(type.Implements(typeof(Entity)))
							LoadEntity(ref script);
					}
					else if(type.Implements(typeof(ScriptCompiler)))
					{
						var compiler = Activator.CreateInstance(type) as ScriptCompiler;
						LoadAssembly(compiler.Compile());
					}
				}

				CompiledScripts.Add(script);
			}
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

		private void LoadEntity(ref CryScript script)
		{
			Entity.RegisterEntityClass(Entity.GetEntityConfig(script.ScriptType));
		}

		private void LoadFlowNode(ref CryScript script, bool entityNode = false)
		{
			bool containsNodePorts = false;
			foreach(var member in script.ScriptType.GetMembers())
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
		public static int LastScriptId = 0;

		internal List<string> FlowNodes;

		/// <summary>
		/// Avoid creating a new empty CryScriptInstance each time we need to check
		/// </summary>
		static CryScriptInstance InvalidScriptInstance = default(CryScriptInstance);

		internal static List<CryScript> CompiledScripts = new List<CryScript>();
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