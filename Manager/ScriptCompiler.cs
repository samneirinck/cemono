using System;
using System.CodeDom.Compiler;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using Microsoft.CSharp;
using System.Diagnostics;
using System.Text;

using CryEngine;

namespace CryMono
{
	/// <summary>
	/// The CryScriptCompiler is responsible for all C# script compilation and plugin loading.
	/// </summary>
	public class CryScriptCompiler : MarshalByRefObject
	{
		public CryScriptCompiler(Manager manager)
		{
            m_compiledScripts = new List<CryScript>();
            m_Manager = manager;

            m_numInstances = 0;

			Reload();
		}

		public void Reload()
		{
			LoadPrecompiledAssemblies();
			LocateAndCompileScripts();
		}

		private void LoadPrecompiledAssemblies()
		{
			//Add pre-compiled assemblies / plugins
			AddScripts(ScriptCompiler.LoadLibrariesInFolder(Path.Combine(CryPath.GetScriptsFolder(), "Plugins")));
		}

		private void LocateAndCompileScripts()
		{
			string[] folders = { Path.Combine(CryPath.GetScriptsFolder(), "Entities"), 
								   Path.Combine(CryPath.GetScriptsFolder(), "GameRules"),
								   Path.Combine(CryPath.GetScriptsFolder(), "FlowNodes") };

			foreach(var folder in folders)
				AddScripts(ScriptCompiler.CompileScriptsInFolder(folder));
		}

		private void AddScripts(CryScript[] scripts)
		{
			if(scripts == null || scripts.Length < 1)
				return;

            foreach (var tempScript in scripts)
            {
                if(tempScript.Type!=null)
                    m_compiledScripts.Add(tempScript);
            }
		}

		/// <summary>
		/// Instantiates a script using its name and interface.
		/// </summary>
		/// <typeparam name="T">An interface representing the type of script. Must itself implement ICryScriptType.</typeparam>
		/// <param name="scriptName"></param>
		/// <returns></returns>
		public int Instantiate<T>(string scriptName) where T : ICryScriptType
		{
            // I can play with sexy lambdas too!
            int index = m_compiledScripts.FindIndex(x => x.Type.Name.Equals(scriptName));
            CryScript script = m_compiledScripts[index];

            if (script.Instances == null)
                script.Instances = new List<CryScriptInstance>();

            m_numInstances++;
            script.Instances.Add(new CryScriptInstance(m_numInstances, Activator.CreateInstance(script.Type)));

            m_compiledScripts[index] = script;

            CryConsole.Log("Instantiated script {0} with an ID of {1}", script.Type.ToString(), m_numInstances.ToString());

            return m_numInstances;
		}

        public void RemoveInstance(int scriptId, string scriptName)
        {
            int index = m_compiledScripts.FindIndex(x => x.Type.Name.Equals(scriptName));
            CryScript script = m_compiledScripts[index];

            int instanceIndex = script.Instances.FindIndex(x => x.Id == scriptId);
            script.Instances.RemoveAt(instanceIndex);

            m_compiledScripts[index] = script;
        }

        public CryScriptInstance GetScriptInstanceById(int id)
		{
            for(int i = 0; i < m_compiledScripts.Count; i++)
            {
                if (m_compiledScripts[i].Instances != null)
                    return m_compiledScripts[i].Instances.Where(instance => instance.Id == id).FirstOrDefault();
            }

            return default(CryScriptInstance);
		}

        public void UpdateScriptsOfType(MonoScriptType scriptType)
        {
            for (int i = 0; i < m_compiledScripts.Count; i++)
            {
                if (m_compiledScripts[i].ScriptType == scriptType && m_compiledScripts[i].Instances != null)
                {
                    for(int index = 0; index < m_compiledScripts[i].Instances.Count; index++)
                        m_Manager.InvokeScriptFunction(m_compiledScripts[i].Instances[index].Instance, "OnUpdate");
                }
            }
        }

        List<CryScript> m_compiledScripts;
        int m_numInstances;

        Manager m_Manager;
	}
}