using System.Reflection;

using System.IO;

using System.Collections;
using System.Collections.Specialized;
using System.Collections.Generic;
using System.Collections.ObjectModel;

using System.Linq;

using System.Runtime.Serialization;

using System.Diagnostics;

using CryEngine.Extensions;

namespace CryEngine.Serialization
{
	/// <summary>
	/// Handles serializing script instances for an app domain reload.
	/// </summary>
	public class AppDomainSerializer
	{
		AppDomainSerializer()
		{
		}

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
					SerializeTypesToXml(script.ScriptInstances, script.ScriptType, compiledScriptsDirectory);
			}

			string subSystemDirectory = Path.Combine(scriptDumpFolder, "CryBrary.EntitySystem");
			Directory.CreateDirectory(subSystemDirectory);

			var processedTypes = new Dictionary<System.Type, string>();

			for(int i = 0; i < EntitySystem.SpawnedEntities.Count; i++)
			{
				var entityType = EntitySystem.SpawnedEntities[i].GetType();

				if(!processedTypes.ContainsKey(entityType))
				{
					string directory = Path.Combine(subSystemDirectory, entityType.Namespace + "." + entityType.Name);
					Directory.CreateDirectory(directory);

					processedTypes.Add(entityType, directory);
				}

				var formatter = new CrySerializer();
				var stream = File.Create(Path.Combine(processedTypes[entityType], Directory.GetFiles(processedTypes[entityType]).Count().ToString()));

				formatter.Serialize(stream, EntitySystem.SpawnedEntities[i]);

				stream.Close();
			}

			stopwatch.Stop();

			Debug.LogAlways("Serializer took {0}ms to dump script data", stopwatch.ElapsedMilliseconds);
		}

		public void SerializeTypesToXml(IEnumerable<object> typeInstances, System.Type type, string targetDirection)
		{
			if(typeInstances.Count() <= 0 || type == null)
				return;

			targetDirection = Directory.CreateDirectory(Path.Combine(targetDirection, type.Namespace + "." + type.Name)).FullName;

			for(int i = 0; i < typeInstances.Count(); i++)
			{
				var formatter = new CrySerializer();
				var stream = File.Create(Path.Combine(targetDirection, i.ToString()));

				formatter.Serialize(stream, typeInstances.ElementAt(i));

				stream.Close();
			}
		}


		public void TrySetScriptData()
		{
			Stopwatch stopwatch = Stopwatch.StartNew();

			string compiledScriptsDirectory = Path.Combine(PathUtils.GetScriptDumpFolder(), "ScriptCompiler.CompiledScripts");

			for(int i = 0; i < ScriptCompiler.CompiledScripts.Count; i++)
			{
				var script = ScriptCompiler.CompiledScripts[i];

				string directoryName = Path.Combine(compiledScriptsDirectory, (script.ScriptType.Namespace + "." + script.ScriptType.Name));
				if(Directory.Exists(directoryName))
				{
					foreach(var fileName in Directory.GetFiles(directoryName))
					{
						if(script.ScriptInstances == null)
							script.ScriptInstances = new Collection<CryScriptInstance>();

						var formatter = new CrySerializer();
						var stream = File.Open(fileName, FileMode.Open);

						var scriptInstance = formatter.Deserialize(stream) as CryScriptInstance;

						if(scriptInstance != null)
							script.ScriptInstances.Add(scriptInstance);

						stream.Close();

						if(ScriptCompiler.NextScriptId <= script.ScriptInstances.Last().ScriptId)
							ScriptCompiler.NextScriptId = script.ScriptInstances.Last().ScriptId + 1;
					}
				}

				ScriptCompiler.CompiledScripts[i] = script;
			}

			string subSystemDirectory = Path.Combine(PathUtils.GetScriptDumpFolder(), "CryBrary.EntitySystem");
			foreach(var directory in Directory.GetDirectories(subSystemDirectory))
			{
				string typeDirectory = new DirectoryInfo(directory).Name;

				System.Type type = null;
				var scriptMatch = ScriptCompiler.CompiledScripts.Find(script => (script.ScriptType.Namespace + "." + script.ScriptType.Name).Equals(typeDirectory));
				if(scriptMatch != default(CryScript))
					type = scriptMatch.ScriptType;

				if(type != null)
				{
					int scriptIndex = ScriptCompiler.CompiledScripts.IndexOf(scriptMatch);

					foreach(var fileName in Directory.GetFiles(directory))
					{
						var formatter = new CrySerializer();
						var stream = File.Open(fileName, FileMode.Open);

						var entity = formatter.Deserialize(stream) as StaticEntity;

						if(entity != null)
						{
							EntitySystem.SpawnedEntities.Add(entity);

							scriptMatch.ScriptInstances.Add(entity);

							if(ScriptCompiler.NextScriptId <= entity.ScriptId)
								ScriptCompiler.NextScriptId = entity.ScriptId + 1;
						}

						ScriptCompiler.CompiledScripts[scriptIndex] = scriptMatch;

						stream.Close();
					}
				}
			}

			stopwatch.Stop();

			Debug.LogAlways("Serializer took {0}ms to set script data", stopwatch.ElapsedMilliseconds);
		}
	}

}
