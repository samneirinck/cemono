using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using CryEngine.Initialization;

namespace CryEngine.Serialization
{
	/// <summary>
	/// Handles serializing script instances for an app domain reload.
	/// </summary>
	public class AppDomainSerializer
	{
		AppDomainSerializer()
		{
			Formatter = new CrySerializer();
		}

		CrySerializer Formatter { get; set; }

		public void DumpScriptData()
		{
			Stopwatch stopwatch = Stopwatch.StartNew();

			string scriptDumpFolder = PathUtils.ScriptDumpFolder;

			if(Directory.Exists(scriptDumpFolder))
				Directory.Delete(scriptDumpFolder, true);

			Directory.CreateDirectory(scriptDumpFolder);

			string compiledScriptsDirectory = Path.Combine(scriptDumpFolder, "ScriptManager.CompiledScripts");
			Directory.CreateDirectory(compiledScriptsDirectory);

			foreach(var script in ScriptManager.CompiledScripts)
			{
				if(script.ScriptInstances != null)
					SerializeTypes(script.ScriptInstances, script.Type, compiledScriptsDirectory);
			}

			stopwatch.Stop();
			Debug.LogAlways("Serializer took {0}ms to dump script data", stopwatch.ElapsedMilliseconds);

			Formatter = null;
		}

		public void SerializeTypes(IEnumerable<object> typeInstances, System.Type type, string targetDirectory)
		{
			if(typeInstances.Count() <= 0 || type == null)
				return;

			targetDirectory = Directory.CreateDirectory(Path.Combine(targetDirectory, type.Namespace + "." + type.Name)).FullName;

			for(int i = 0; i < typeInstances.Count(); i++)
			{
				using(var stream = File.Create(Path.Combine(targetDirectory, i.ToString())))
					Formatter.Serialize(stream, typeInstances.ElementAt(i));
			}
		}

		public void TrySetScriptData()
		{
			Stopwatch stopwatch = Stopwatch.StartNew();

			string compiledScriptsDirectory = Path.Combine(PathUtils.ScriptDumpFolder, "ScriptManager.CompiledScripts");

			for(int i = 0; i < ScriptManager.CompiledScripts.Count; i++)
			{
				var script = ScriptManager.CompiledScripts[i];

				string directoryName = Path.Combine(compiledScriptsDirectory, (script.Type.Namespace + "." + script.Type.Name));
				if(Directory.Exists(directoryName))
				{
					foreach(var fileName in Directory.GetFiles(directoryName))
					{
						if(script.ScriptInstances == null)
							script.ScriptInstances = new List<CryScriptInstance>();

						CryScriptInstance scriptInstance = null;
						using(var stream = File.Open(fileName, FileMode.Open))
							scriptInstance = Formatter.Deserialize(stream) as CryScriptInstance;

						if(scriptInstance != null)
						{
							script.ScriptInstances.Add(scriptInstance);

							if(ScriptManager.LastScriptId <= scriptInstance.ScriptId)
								ScriptManager.LastScriptId = scriptInstance.ScriptId + 1;
						}
					}
				}

				ScriptManager.CompiledScripts[i] = script;
			}

			stopwatch.Stop();

			Debug.LogAlways("Serializer took {0}ms to set script data", stopwatch.ElapsedMilliseconds);
		}
	}

}
