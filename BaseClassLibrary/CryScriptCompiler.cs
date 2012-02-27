using System.Linq;
using System.IO;
using System.Collections.Generic;

using System.Reflection;

using CryEngine.Extensions;
using CryEngine.Utils;

using System.ComponentModel;
using System.Threading.Tasks;

using System.Xml;
using System.Xml.Linq;

namespace CryEngine
{
	/// <summary>
	/// The script compiler is responsible for all CryScript compilation.
	/// </summary>
	public static partial class ScriptCompiler
	{
		public static void Initialize()
		{
#if !RELEASE
			//Pdb2Mdb.Driver.Convert(Assembly.GetExecutingAssembly());
#endif

			CompiledScripts = new List<CryScript>();
			FlowNodes = new List<StoredNode>();

			AssemblyReferenceHandler.Initialize();			

			NextScriptId = 0;

			//GenerateScriptbindAssembly(scriptBinds.ToArray());

			AssemblyReferenceHandler.ReferencedAssemblies.AddRange(System.AppDomain.CurrentDomain.GetAssemblies().Select(a => a.Location).ToArray());

			LoadPrecompiledAssemblies();

			AddScripts(CompileScriptsInFolders(
				PathUtils.GetScriptFolder(MonoScriptType.Entity),
				PathUtils.GetScriptFolder(MonoScriptType.GameRules),
				PathUtils.GetScriptFolder(MonoScriptType.FlowNode)
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

		private static void AddScripts(CryScript[] scripts)
		{
			if (scripts == null || scripts.Length < 1)
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
			if (scriptName.Length < 1)
			{
				Console.LogAlways("Empty script passed to InstantiateClass");

				return null;
			}

			// I can play with sexy lambdas too!
			int index = CompiledScripts.FindIndex(x => x.className.Equals(scriptName));
			if (index == -1)
			{
				Console.LogAlways("Failed to instantiate {0}, compiled script could not be found.", scriptName);

				return null;
			}

			CryScript script = CompiledScripts.ElementAt(index);
			//if (!script.Type.Implements(typeof(CryScriptInstance)))
			//	return null;

			NextScriptId++;

			if (script.ScriptInstances == null)
				script.ScriptInstances = new List<CryScriptInstance>();

			script.ScriptInstances.Add(System.Activator.CreateInstance(script.Type, constructorParams) as CryScriptInstance);
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
			int scriptIndex = CompiledScripts.FindIndex(x => x.className == instance.GetType().Name);
			if (scriptIndex != -1)
			{
				CryScript script = CompiledScripts.ElementAt(scriptIndex);

				if (script.ScriptInstances == null)
					script.ScriptInstances = new List<CryScriptInstance>();
				else if (script.ScriptInstances.Contains(instance))
					return -1;

				NextScriptId++;

				instance.ScriptId = NextScriptId;
				script.ScriptInstances.Add(instance);

				CompiledScripts[scriptIndex] = script;

				return NextScriptId;
			}

			Console.LogAlways("Couldn't add script {0}", instance.GetType().Name);
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
			if (scriptName.Length > 0)
			{
				int index = CompiledScripts.FindIndex(x => x.className.Equals(scriptName));
				if (index == -1)
					return;

				CryScript script = CompiledScripts[index];

				if (script.ScriptInstances != null)
				{
					int instanceIndex = script.ScriptInstances.FindIndex(x => x.ScriptId == scriptId);
					if (instanceIndex == -1)
					{
						Console.LogAlways("Failed to remove script of type {0} with id {1}; instance was not found.", scriptName, scriptId);
						return;
					}

					script.ScriptInstances.RemoveAt(instanceIndex);

					CompiledScripts[index] = script;
				}
			}
			else
			{
				for (int i = 0; i < CompiledScripts.Count; i++)
				{
					CryScript script = CompiledScripts[i];

					if (script.ScriptInstances != null)
					{
						int scriptIndex = script.ScriptInstances.FindIndex(x => x.ScriptId == scriptId);
						if (scriptIndex != -1)
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
			if (scriptInstance == default(CryScriptInstance))
			{
				Console.LogAlways("Failed to invoke method, script instance was invalid");
				return null;
			}

			return InvokeScriptFunction(scriptInstance, func, args);
		}

		public static CryScriptInstance GetScriptInstanceById(int id)
		{
			var scripts = CompiledScripts.Where(script => script.ScriptInstances != null);

			CryScriptInstance scriptInstance = null;
			foreach (var script in scripts)
			{
				scriptInstance = script.ScriptInstances.FirstOrDefault(instance => instance.ScriptId == id);

				if (scriptInstance != default(CryScriptInstance))
					return scriptInstance;
			}

			return null;
		}

		public static int GetEntityScriptId(EntityId entityId, System.Type scriptType = null)
		{
			var scripts = CompiledScripts.Where(script => (scriptType != null ? script.Type.Implements(scriptType) : true) && script.ScriptInstances != null);

			foreach (var compiledScript in scripts)
			{
				foreach (var script in compiledScript.ScriptInstances)
				{
					var scriptEntity = script as StaticEntity;
					if (scriptEntity != null && scriptEntity.Id == entityId)
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
			if (ScriptBinds == null)
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
				if(script.ScriptInstances!=null)
					script.ScriptInstances.Where(i => i.ReceiveUpdates).ToList().ForEach(i => i.OnUpdate());
			});

			EntitySystem.OnUpdate();
		}

		static List<Scriptbind> ScriptBinds;

		public static List<CryScript> CompiledScripts;
		public static int NextScriptId;
	}
}
