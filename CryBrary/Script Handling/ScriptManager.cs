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
		internal ScriptManager()
		{
			if(!Directory.Exists(PathUtils.GetTempFolder()))
				Directory.CreateDirectory(PathUtils.GetTempFolder());

			foreach(var file in Directory.GetFiles(PathUtils.GetTempFolder()))
				File.Delete(file);

			FlowNodes = new List<string>();

			Type[] specialTypes = { typeof(NativeEntity), typeof(NativeActor) };
			foreach(var type in specialTypes)
				CompiledScripts.Add(new CryScript(type));
		}

		bool LoadPlugins()
		{
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

				if(foundScript.ScriptName.Equals(scriptName) || (foundScript.ScriptName.Contains(scriptName) && foundScript.Type.Name.Equals(scriptName)))
				{
					script = foundScript;
					break;
				}
			}

			if(script == default(CryScript))
				throw new ScriptNotFoundException(string.Format("Compiled script {0} could not be found.", scriptName));

			return AddScriptInstance(System.Activator.CreateInstance(script.Type, constructorParams) as CryScriptInstance, script, scriptIndex);
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

		public int GetEntityScriptId(EntityId entityId, System.Type scriptType = null)
		{
			var scripts = CompiledScripts.Where(script => (scriptType != null ? script.Type.Implements(scriptType) : true) && script.ScriptInstances != null);

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
			ProcessTypes(assembly.GetTypes());
		}

		/// <summary>
		/// Loads a C# assembly by location, creates a shadow-copy & generates debug database (mdb).
		/// </summary>
		/// <param name="currentLocation"></param>
		public void LoadAssembly(string assemblyPath)
		{
			var newPath = Path.Combine(PathUtils.GetTempFolder(), Path.GetFileName(assemblyPath));
			File.Copy(assemblyPath, newPath, true);

#if !RELEASE
			GenerateDebugDatabaseForAssembly(assemblyPath);

			var mdbFile = assemblyPath + ".mdb";
			if(File.Exists(mdbFile)) // success
				File.Copy(mdbFile, Path.Combine(Path.GetTempPath(), Path.GetFileName(mdbFile)), true);
#endif

			ProcessAssembly(Assembly.LoadFrom(newPath));
		}

		/// <summary>
		/// Processes a type and adds all found types to ScriptCompiler.CompiledScripts
		/// </summary>
		/// <param name="types"></param>
		void ProcessTypes(IEnumerable<Type> types)
		{
			foreach(var type in types)
			{
				var script = new CryScript(type);

				if(!type.IsAbstract && !type.ContainsAttribute<ExcludeFromCompilationAttribute>())
				{
					switch(script.ScriptType)
					{
						case ScriptType.Actor:
							Actor._RegisterActorClass(script.ScriptName, false);
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

				CompiledScripts.Add(script);
			}
		}

		void LoadEntity(ref CryScript script)
		{
			LoadFlowNode(ref script);

			Entity.RegisterClass(Entity.GetEntityConfig(script.Type));
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
				var assembly = Assembly.LoadFrom(Path.Combine(PathUtils.GetEngineFolder(), "Mono", "bin", "pdb2mdb.dll"));
				var driver = assembly.GetType("Driver");
				var convertMethod = driver.GetMethod("Convert", BindingFlags.Static | BindingFlags.Public);

				object[] args = { assemblyPath };
				convertMethod.Invoke(null, args);
			}
		}

		List<string> FlowNodes { get; set; }

		#region Statics
		internal static CryScript GetScriptByType(Type type, out int scriptIndex)
		{
			var script = default(CryScript);
			scriptIndex = 0;
			for(; scriptIndex < CompiledScripts.Count; scriptIndex++)
			{
				var foundScript = CompiledScripts[scriptIndex];

				if(foundScript.Type == type)
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

		/// <summary>
		/// Avoid creating a new empty CryScriptInstance each time we need to check
		/// </summary>
		static CryScriptInstance InvalidScriptInstance = default(CryScriptInstance);

		/// <summary>
		/// Last assigned ScriptId, next = + 1
		/// </summary>
		public static int LastScriptId = 0;

		internal static List<CryScript> CompiledScripts = new List<CryScript>();
		#endregion
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