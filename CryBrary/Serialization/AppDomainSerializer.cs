using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using CryEngine.Initialization;

namespace CryEngine.Serialization
{
	/// <summary>
	/// Handles serializing script instances for an app domain reload.
	/// </summary>
	public class AppDomainSerializer
	{
		public AppDomainSerializer()
		{
			Formatter = new CrySerializer();
		}

		CrySerializer Formatter { get; set; }

		public void DumpScriptData()
		{
			Stopwatch stopwatch = Stopwatch.StartNew();

			using(var stream = File.Create(Path.Combine(PathUtils.TempFolder, "ScriptManager.CompiledScripts.scriptdump")))
				Formatter.Serialize(stream, ScriptManager.Scripts);

			stopwatch.Stop();
			Debug.LogAlways("Serializer took {0}ms to dump script data", stopwatch.ElapsedMilliseconds);

			Formatter = null;
		}

		public void TrySetScriptData()
		{
			Stopwatch stopwatch = Stopwatch.StartNew();

			using(var stream = File.Open(Path.Combine(PathUtils.TempFolder, "ScriptManager.CompiledScripts.scriptdump"), FileMode.Open))
				ScriptManager.Scripts = Formatter.Deserialize(stream) as List<CryScript>;

			ScriptManager.ForEach(ScriptType.Any, scriptInstance =>
			{
				if(scriptInstance.ScriptId > ScriptManager.LastScriptId)
					ScriptManager.LastScriptId = scriptInstance.ScriptId + 1;
			});

			stopwatch.Stop();

			Debug.LogAlways("Serializer took {0}ms to set script data", stopwatch.ElapsedMilliseconds);
		}
	}

}
