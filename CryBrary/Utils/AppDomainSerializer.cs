using System.Reflection;

using System.IO;
using System.Security.AccessControl;

using System.Collections;
using System.Collections.ObjectModel;
using System.Collections.Generic;

using System.Linq;

using System.Xml;
using System.Xml.Linq;

using System.Xml.Serialization;

using System.Text;

using CryEngine.Extensions;

namespace CryEngine.Utils
{
	/// <summary>
	/// Handles serializing script instances for an app domain reload.
	/// </summary>
	public static class AppDomainSerializer
	{
		public static void DumpScriptData()
		{
			string tempDirectory = Path.Combine(PathUtils.GetRootFolder(), "Temp");

			string compiledScriptsDirectory = Path.Combine(tempDirectory, "ScriptCompiler.CompiledScripts");
			if(Directory.Exists(compiledScriptsDirectory))
				Directory.Delete(compiledScriptsDirectory, true);

			Directory.CreateDirectory(compiledScriptsDirectory);

			foreach(var script in ScriptCompiler.CompiledScripts)
			{
				if(script.ScriptInstances != null)
					SerializeTypesToXml(script.ScriptInstances, script.ClassType, compiledScriptsDirectory);
			}

			string subSystemDirectory = Path.Combine(tempDirectory, "CryBrary.EntitySystem");
			if(Directory.Exists(subSystemDirectory))
				Directory.Delete(subSystemDirectory, true);

			Directory.CreateDirectory(subSystemDirectory);

			var xmlSettings = new XmlWriterSettings();
			xmlSettings.Indent = true;

			Dictionary<System.Type, List<object>> types = new Dictionary<System.Type,List<object>>();

			foreach(var entity in EntitySystem.SpawnedEntities)
			{
				var type = entity.GetType();

				if(!types.ContainsKey(type))
					types.Add(type, new List<object>());

				types[type].Add(entity);
			}

			foreach(var type in types)
			{
				string directory = Path.Combine(subSystemDirectory, type.Key.FullName);
				Directory.CreateDirectory(directory);

				XmlSerializer xmlSerializer = new XmlSerializer(type.Key);

				for(int i = 0; i < type.Value.Count; i++)
				{
					using(XmlWriter writer = XmlWriter.Create(Path.Combine(directory, i.ToString()), xmlSettings))
						xmlSerializer.Serialize(writer, type.Value.ElementAt(i));
				}
			}
		}

		public static void SerializeSubsystemToXml(System.Type subSystem, string directory)
		{
			directory = Path.Combine(directory, subSystem.Name);
			Directory.CreateDirectory(directory);

			var xmlSettings = new XmlWriterSettings();
			xmlSettings.Indent = true;

			XmlSerializer xmlSerializer = new XmlSerializer(subSystem);

			using(XmlWriter writer = XmlWriter.Create(Path.Combine(directory, subSystem.Name), xmlSettings))
				xmlSerializer.Serialize(writer, null);
		}

		public static void SerializeTypesToXml(IEnumerable<object> typeInstances, System.Type type, string targetDir)
		{
			if(typeInstances.Count() <= 0)
				return;

			var xmlSettings = new XmlWriterSettings();
			xmlSettings.Indent = true;

			targetDir = Directory.CreateDirectory(Path.Combine(targetDir, type.Name)).FullName;

			XmlSerializer xmlSerializer = new XmlSerializer(type);

			for(int i = 0; i < typeInstances.Count(); i++)
			{
				using(XmlWriter writer = XmlWriter.Create(Path.Combine(targetDir, i.ToString()), xmlSettings))
					xmlSerializer.Serialize(writer, typeInstances.ElementAt(i));
			}
		}


		public static void TrySetScriptData()
		{
			string tempDirectory = Path.Combine(PathUtils.GetRootFolder(), "Temp");

			string compiledScriptsDirectory = Path.Combine(tempDirectory, "ScriptCompiler.CompiledScripts");

			for(int i = 0; i < ScriptCompiler.CompiledScripts.Count; i++)
			{
				var script = ScriptCompiler.CompiledScripts[i];
				XmlSerializer xmlSerializer = new XmlSerializer(script.ClassType);

				string directoryName = Path.Combine(compiledScriptsDirectory, script.ClassName);
				if(Directory.Exists(directoryName))
				{
					foreach(var fileName in Directory.GetFiles(directoryName))
					{
						using(XmlReader reader = XmlReader.Create(fileName))
						{
							if(xmlSerializer.CanDeserialize(reader))
							{
								if(script.ScriptInstances == null)
									script.ScriptInstances = new Collection<CryScriptInstance>();

								script.ScriptInstances.Add(xmlSerializer.Deserialize(reader) as CryScriptInstance);

								if(ScriptCompiler.NextScriptId <= script.ScriptInstances.Last().ScriptId)
									ScriptCompiler.NextScriptId = script.ScriptInstances.Last().ScriptId + 1;
							}
						}
					}
				}

				ScriptCompiler.CompiledScripts[i] = script;
			}

			string subSystemDirectory = Path.Combine(tempDirectory, "CryBrary.EntitySystem");
			foreach(var directory in Directory.GetDirectories(subSystemDirectory))
			{
				string typeDirectory = new DirectoryInfo(directory).Name;

				System.Type type = null;
				var scriptMatch = ScriptCompiler.CompiledScripts.Find(script => script.ClassType.FullName.Equals(typeDirectory));
				if(scriptMatch != default(CryScript))
					type = scriptMatch.ClassType;

				if(type != null)
				{
					XmlSerializer xmlSerializer = new XmlSerializer(type);

					foreach(var fileName in Directory.GetFiles(directory))
					{
						using(XmlReader reader = XmlReader.Create(fileName))
						{
							if(xmlSerializer.CanDeserialize(reader))
								EntitySystem.SpawnedEntities.Add(xmlSerializer.Deserialize(reader) as StaticEntity);
						}
					}
				}
			}
		}
	}
}
