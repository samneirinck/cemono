using System.IO;

using System.Collections.Generic;
using System.Collections.ObjectModel;

using System.Linq;

using System.Diagnostics;

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

			string scriptDumpFolder = PathUtils.GetScriptDumpFolder();

			if(Directory.Exists(scriptDumpFolder))
				Directory.Delete(scriptDumpFolder, true);

			Directory.CreateDirectory(scriptDumpFolder);

			string compiledScriptsDirectory = Path.Combine(scriptDumpFolder, "ScriptCompiler.CompiledScripts");
			Directory.CreateDirectory(compiledScriptsDirectory);

			foreach(var script in ScriptCompiler.CompiledScripts)
			{
				if(script.ScriptInstances != null)
					SerializeTypes(script.ScriptInstances, script.ScriptType, compiledScriptsDirectory);
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

			string compiledScriptsDirectory = Path.Combine(PathUtils.GetScriptDumpFolder(), "ScriptCompiler.CompiledScripts");

			for(int i = 0; i < ScriptCompiler.CompiledScripts.Length; i++)
			{
				var script = ScriptCompiler.CompiledScripts[i];

				string directoryName = Path.Combine(compiledScriptsDirectory, (script.ScriptType.Namespace + "." + script.ScriptType.Name));
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
							script.ScriptInstances.Add(scriptInstance);

						if(ScriptCompiler.LastScriptId <= script.ScriptInstances.Last().ScriptId)
							ScriptCompiler.LastScriptId = script.ScriptInstances.Last().ScriptId + 1;
					}
				}

				ScriptCompiler.CompiledScripts[i] = script;
			}

			stopwatch.Stop();

			Debug.LogAlways("Serializer took {0}ms to set script data", stopwatch.ElapsedMilliseconds);
		}
	}

}
