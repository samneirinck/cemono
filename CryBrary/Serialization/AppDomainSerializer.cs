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

			if(Directory.Exists(PathUtils.ScriptDumpFolder))
				Directory.Delete(PathUtils.ScriptDumpFolder, true);

			Directory.CreateDirectory(PathUtils.ScriptDumpFolder);

			using(var stream = File.Create(Path.Combine(PathUtils.ScriptDumpFolder, "ScriptManager.CompiledScripts.scriptdump")))
				Formatter.Serialize(stream, ScriptManager.CompiledScripts);

			stopwatch.Stop();
			Debug.LogAlways("Serializer took {0}ms to dump script data", stopwatch.ElapsedMilliseconds);

			Formatter = null;
		}

		public void TrySetScriptData()
		{
			Stopwatch stopwatch = Stopwatch.StartNew();

			using(var stream = File.Open(Path.Combine(PathUtils.ScriptDumpFolder, "ScriptManager.CompiledScripts.scriptdump"), FileMode.Open))
				ScriptManager.CompiledScripts = Formatter.Deserialize(stream) as Dictionary<ScriptType, List<CryScript>>;

			// TODO: Get highest script id and set LastScriptId to that + 1.
			//ScriptManager.LastScriptId = 

			stopwatch.Stop();

			Debug.LogAlways("Serializer took {0}ms to set script data", stopwatch.ElapsedMilliseconds);
		}
	}

}
