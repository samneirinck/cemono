using System;
using System.Linq;
using System.IO;
using System.Collections.Generic;

using System.Reflection;
using System.Runtime.CompilerServices;

using CryEngine.Extensions;

using System.ComponentModel;
using System.Threading.Tasks;

namespace CryEngine
{
	/// <summary>
	/// The script compiler is responsible for all CryScript compilation.
	/// </summary>
	public class ScriptCompiler : _ScriptCompiler
	{
		/// <summary>
		/// Requests a full reload of all C# scripts and dll's, including CryBrary.
		/// </summary>
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void RequestReload();

		public ScriptCompiler()
		{
#if !RELEASE
			Pdb2Mdb.Driver.Convert(Assembly.GetExecutingAssembly());
#endif

			m_compiledScripts = new List<CryScript>();
			m_flowNodes = new List<StoredNode>();
			referencedAssemblies = new List<string>();

			m_numInstances = 0;
		}

		public void Initialize()
		{
			//GenerateScriptbindAssembly(scriptBinds.ToArray());

			referencedAssemblies.AddRange(AppDomain.CurrentDomain.GetAssemblies().Select(a => a.Location).ToArray());

			LoadPrecompiledAssemblies();

			AddScripts(CompileScriptsInFolders(
				PathUtils.GetScriptFolder(MonoScriptType.Entity),
				PathUtils.GetScriptFolder(MonoScriptType.GameRules),
				PathUtils.GetScriptFolder(MonoScriptType.FlowNode)
			));
		}

		public void PostInit()
		{
			// These have to be registered later on due to the flow system being initialized late.
			RegisterFlownodes();
		}

		private void LoadPrecompiledAssemblies()
		{
			//Add pre-compiled assemblies / plugins
			AddScripts(ScriptCompiler.LoadLibrariesInFolder(Path.Combine(PathUtils.GetScriptsFolder(), "Plugins")));
		}

		private void AddScripts(CryScript[] scripts)
		{
			if (scripts == null || scripts.Length < 1)
				return;

			m_compiledScripts.AddRange(scripts);
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
			if (scriptName.Length < 1)
			{
				Console.LogAlways("Empty script passed to InstantiateClass");

				return null;
			}

			// I can play with sexy lambdas too!
			int index = m_compiledScripts.FindIndex(x => x.className.Equals(scriptName));
			if (index == -1)
			{
				Console.LogAlways("Failed to instantiate {0}, compiled script could not be found.", scriptName);

				return null;
			}

			CryScript script = m_compiledScripts.ElementAt(index);
			if (!script.Type.Implements(typeof(CryScriptInstance)))
				return null;


			m_numInstances++;
			//ScriptId

			script.ScriptInstances.Add(Activator.CreateInstance(script.Type, constructorParams) as CryScriptInstance);
			script.ScriptInstances.Last().ScriptId = m_numInstances;

			m_compiledScripts[index] = script;

			return script.ScriptInstances.Last();
		}

		[EditorBrowsable(EditorBrowsableState.Never)]
		public void RemoveInstance(int scriptId, string scriptName)
		{
			int index = m_compiledScripts.FindIndex(x => x.className.Equals(scriptName));
			if (index == -1)
				return;

			CryScript script = m_compiledScripts[index];

			int instanceIndex = script.ScriptInstances.FindIndex(x => x.ScriptId == scriptId);
			script.ScriptInstances.RemoveAt(instanceIndex);

			m_compiledScripts[index] = script;

			// Allow re-use of Id's.
			if (m_numInstances == scriptId)
				m_numInstances--;
		}

		[EditorBrowsable(EditorBrowsableState.Never)]
		public object InvokeScriptFunctionById(int id, string func, object[] args = null)
		{
			CryScriptInstance scriptInstance = GetScriptInstanceById(id);
			if (scriptInstance == default(CryScriptInstance))
			{
				Console.LogAlways("Failed to invoke method, script instance was invalid");
				return null;
			}

			return InvokeScriptFunction(scriptInstance, func, args);
		}

		public CryScriptInstance GetScriptInstanceById(int id)
		{
			for (int i = 0; i < m_compiledScripts.Count; i++)
			{
				if (m_compiledScripts[i].ScriptInstances != null)
				{
					CryScriptInstance tempInstance = m_compiledScripts[i].ScriptInstances.FirstOrDefault(instance => instance.ScriptId == id);

					if (tempInstance != default(CryScriptInstance))
						return tempInstance;
				}
			}

			return null;
		}

		/// <summary>
		/// Automagically registers scriptbind methods to rid us of having to add them in both C# and C++.
		/// </summary>
		[EditorBrowsable(EditorBrowsableState.Never)]
		public void RegisterScriptbind(string namespaceName, string className, object[] methods)
		{
			if (scriptBinds == null)
				scriptBinds = new List<Scriptbind>();

			scriptBinds.Add(new Scriptbind(namespaceName, className, methods));
		}

		public void OnFileChange(string filePath)
		{
			string file = filePath.Split('/').Last();

			if (file.Contains(".cs"))
			{
				file = file.Split('.').First();

				RequestReload();
				// TODO: Script reloading
			}
			else if (file.Contains(".dll"))
			{
				file = file.Split('.').First();
				// TODO: Dll reloading
			}
		}

		/// <summary>
		/// Called once per frame.
		/// </summary>
		public void OnUpdate(float frameTime)
		{
			Time.DeltaTime = frameTime;

			Parallel.ForEach(m_compiledScripts, script =>
			{
				script.ScriptInstances.Where(i => i.ReceiveUpdates).ToList().ForEach(i => i.OnUpdate());
			});
		}

		List<Scriptbind> scriptBinds;

		List<CryScript> m_compiledScripts;
		int m_numInstances;
	}
}
