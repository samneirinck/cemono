using System.Reflection;

using System.IO;

using System.Collections;
using System.Collections.Specialized;
using System.Collections.ObjectModel;
using System.Collections.Generic;

using System.Linq;
using System.Xml;

using System.Xml.Serialization;
using System.Runtime.Serialization;

using CryEngine.Extensions;

namespace CryEngine.Utils
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
			string scriptDumpFolder = PathUtils.GetScriptDumpFolder();

			if(Directory.Exists(scriptDumpFolder))
				Directory.Delete(scriptDumpFolder, true);

			Directory.CreateDirectory(scriptDumpFolder);

			string compiledScriptsDirectory = Path.Combine(scriptDumpFolder, "ScriptCompiler.CompiledScripts");
			Directory.CreateDirectory(compiledScriptsDirectory);

			foreach(var script in ScriptCompiler.CompiledScripts)
			{
				if(script.ScriptInstances != null)
					SerializeTypesToXml(script.ScriptInstances, script.ClassType, compiledScriptsDirectory);
			}

			string subSystemDirectory = Path.Combine(scriptDumpFolder, "CryBrary.EntitySystem");
			Directory.CreateDirectory(subSystemDirectory);

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
				string directory = Path.Combine(subSystemDirectory, type.Key.Namespace + "." + type.Key.Name);
				Directory.CreateDirectory(directory);

				for(int i = 0; i < type.Value.Count; i++)
				{
					var formatter = new CrySerializer();
					var stream = File.Create(Path.Combine(directory, i.ToString()));

					formatter.Serialize(stream, type.Value.ElementAt(i));

					stream.Close();
				}
			}
		}

		public void SerializeTypesToXml(IEnumerable<object> typeInstances, System.Type type, string targetDir)
		{
			if(typeInstances.Count() <= 0)
				return;

			targetDir = Directory.CreateDirectory(Path.Combine(targetDir, type.Namespace + "." + type.Name)).FullName;

			for(int i = 0; i < typeInstances.Count(); i++)
			{
				var formatter = new CrySerializer();
				var stream = File.Create(Path.Combine(targetDir, i.ToString()));

				formatter.Serialize(stream, typeInstances.ElementAt(i));

				stream.Close();
			}
		}


		public void TrySetScriptData()
		{
			string compiledScriptsDirectory = Path.Combine(PathUtils.GetScriptDumpFolder(), "ScriptCompiler.CompiledScripts");

			for(int i = 0; i < ScriptCompiler.CompiledScripts.Count; i++)
			{
				var script = ScriptCompiler.CompiledScripts[i];

				string directoryName = Path.Combine(compiledScriptsDirectory, (script.ClassType.Namespace + "." + script.ClassType.Name));
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
				var scriptMatch = ScriptCompiler.CompiledScripts.Find(script => (script.ClassType.Namespace + "." + script.ClassType.Name).Equals(typeDirectory));
				if(scriptMatch != default(CryScript))
					type = scriptMatch.ClassType;

				if(type != null)
				{
					foreach(var fileName in Directory.GetFiles(directory))
					{
						var formatter = new CrySerializer();
						var stream = File.Open(fileName, FileMode.Open);

						var entity = formatter.Deserialize(stream) as StaticEntity;

						if(entity != null)
							EntitySystem.SpawnedEntities.Add(entity);

						stream.Close();
					}
				}
			}
		}
	}

	public class CrySerializer : IFormatter
	{
		SerializationBinder binder;
		StreamingContext context;
		ISurrogateSelector surrogateSelector;

		public CrySerializer()
		{
			context = new StreamingContext(StreamingContextStates.All);
		}

		public object Deserialize(Stream serializationStream)
		{
			StreamReader sr = new StreamReader(serializationStream);

			// Get Type from serialized data.
			string line = sr.ReadLine();
			char[] delim = new char[] { '=' };
			string[] sarr = line.Split(delim);
			string className = sarr[1];

			System.Type type = null;
			foreach(var script in ScriptCompiler.CompiledScripts)
			{
				if(script.ClassType.FullName.Equals(className))
				{
					type = script.ClassType;
					break;
				}
			}

			type = type ?? System.Type.GetType(className);

			if(type == null)
				return null;

			ObjectReference reference = null;

			if(type.GetConstructor(System.Type.EmptyTypes) != null || type.IsValueType)
				reference = new ObjectReference(System.Activator.CreateInstance(type));
			else
				Debug.Log("[Warning] Could not serialize type {0} since it did not contain an parameterless constructor", type.Name);

			if(reference == null || reference.Object == null)
				return null;

			if(ObjectReferences.ContainsKey(reference))
				return reference.Object;

			ObjectReferences.Add(reference, ((FileStream)serializationStream).Name);
			int referenceIndex = ObjectReferences.Count - 1;

			// Store serialized variable name -> value pairs.
			StringDictionary sdict = new StringDictionary();
			while(sr.Peek() >= 0)
			{
				line = sr.ReadLine();
				sarr = line.Split(delim);

				// key = variable name, value = variable value.
				sdict[sarr[0].Trim()] = sarr[1].Trim();
			}
			sr.Close();

			if(type.Implements(typeof(IEnumerable)))
			{
				var array = (reference.Object as IEnumerable).Cast<object>().ToArray();

				foreach(var dict in sdict.Keys)
				{
					string indexVal = (string)sdict[(string)dict];

					if(indexVal.StartsWith(ReferenceSeperator) && indexVal.EndsWith(ReferenceSeperator))
					{
						// This was a reference to a serialized object in another file, lets localize it and deserialize.
						string referenceFile = indexVal.Replace(ReferenceSeperator, "");

						var formatter = new CrySerializer();
						var stream = File.Open(referenceFile, FileMode.Open);

						array.SetValue(formatter.Deserialize(stream), System.Convert.ToInt32((string)dict));

						stream.Close();
					}
					else
						array.SetValue(Convert.ChangeType(indexVal, array.GetType().GetElementType()), System.Convert.ToInt32((string)dict) /* index as a string */);
				}

				reference.Object = array;
			}
			else if(!type.IsPrimitive && !type.IsEnum && type != typeof(string))
			{
				FieldInfo[] fields = null;
				while(type != null)
				{
					foreach(var fieldInfo in type.GetFields(BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.DeclaredOnly))
					{
						if(sdict.ContainsKey(fieldInfo.Name))
						{
							if(sdict[fieldInfo.Name].StartsWith(ReferenceSeperator) && sdict[fieldInfo.Name].EndsWith(ReferenceSeperator))
							{
								// This was a reference to a serialized object in another file, lets localize it and deserialize.
								string referenceFile = sdict[fieldInfo.Name].Replace(ReferenceSeperator, "");

								var formatter = new CrySerializer();
								var stream = File.Open(referenceFile, FileMode.Open);

								object deserializedObject = formatter.Deserialize(stream);

								stream.Close();

								fieldInfo.SetValue(reference.Object, deserializedObject);
							}
							else
								fieldInfo.SetValue(reference.Object, Convert.ChangeType(sdict[fieldInfo.Name], fieldInfo.FieldType));
						}
					}

					type = type.BaseType;
				}
			}

			return reference.Object;
		}

		public void Serialize(Stream serializationStream, object objectInstance)
		{
			Serialize(serializationStream, new ObjectReference(objectInstance));
		}

		void Serialize(Stream serializationStream, ObjectReference objectReference)
		{
			// Write class name and all fields & values to file
			StreamWriter sw = new StreamWriter(serializationStream);

			if(ObjectReferences.ContainsKey(objectReference))
			{
				sw.WriteLine("@ClassName=" + ReferenceSeperator + "{0}" + ReferenceSeperator, ObjectReferences[objectReference]);
				sw.Close();

				return;
			}

			sw.WriteLine("@ClassName={0}", objectReference.Type.FullName);

			// Should definitely not hardcode FileStream like this
			ObjectReferences.Add(objectReference, ((FileStream)serializationStream).Name);

			if(objectReference.Type.Implements(typeof(IEnumerable)))
			{
				var array = (objectReference.Object as IEnumerable).Cast<object>();

				for(int i = 0; i < array.Count(); i++)
				{
					object indexedItem = array.ElementAt(i);

					if(indexedItem != null)
						WriteReference(indexedItem, i.ToString(), ref sw);
				}
			}
			else if(!objectReference.Type.IsPrimitive && !objectReference.Type.IsEnum && objectReference.Type != typeof(string))
			{
				BindingFlags bindingFlags = BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.DeclaredOnly;

				var type = objectReference.Type;

				while(type != null)
				{
					foreach(var field in type.GetFields(bindingFlags))
					{
						object fieldValue = field.GetValue(objectReference.Object);

						if(fieldValue != null)
							WriteReference(fieldValue, field.Name, ref sw);
					}

					type = type.BaseType;
				}
			}
			else
				throw new System.Exception(string.Format("This should not happen! Hand this data over to the nearest highly trained monkey: {1}{1}{0}{1}{1}", objectReference.Type.FullName, objectReference.Type.GetHashCode()));

			sw.Close();
		}

		void WriteReference(object objectInstance, string name, ref StreamWriter sw)
		{
			ObjectReference reference = new ObjectReference(objectInstance);
			
			if(!reference.Type.IsPrimitive && !reference.Type.IsEnum && reference.Type != typeof(string))
			{
				// We can't simply write value.ToString() here, write to file and leave the path within brackets to reference it.
				if(!ObjectReferences.ContainsKey(reference))
				{
					string targetDir = Path.Combine(PathUtils.GetScriptDumpFolder(), "Objects");
					if(!Directory.Exists(targetDir))
						Directory.CreateDirectory(targetDir);

					targetDir = Path.Combine(targetDir, reference.Type.Namespace + "." + reference.Type.Name);
					if(!Directory.Exists(targetDir))
						Directory.CreateDirectory(targetDir);

					var stream = File.Create(Path.Combine(targetDir, Directory.GetFiles(targetDir).Length.ToString()));

					var serializer = new CrySerializer();
					serializer.Serialize(stream, reference);

					stream.Close();

					sw.WriteLine("{0}={1}", name, string.Format(ReferenceSeperator + "{0}" + ReferenceSeperator, ObjectReferences[reference]));
				}
				else // Reference already existed, write path to its script dump.
					sw.WriteLine("{0}={1}", name, string.Format(ReferenceSeperator + "{0}" + ReferenceSeperator, ObjectReferences[reference]));
			}
			else
				sw.WriteLine("{0}={1}", name, objectInstance.ToString());
		}

		public ISurrogateSelector SurrogateSelector
		{
			get { return surrogateSelector; }
			set { surrogateSelector = value; }
		}
		public SerializationBinder Binder
		{
			get { return binder; }
			set { binder = value; }
		}
		public StreamingContext Context
		{
			get { return context; }
			set { context = value; }
		}

		internal class ObjectReference
		{
			public ObjectReference(object obj)
			{
				Object = obj;
				Type = obj.GetType();
			}

			public object Object { get; set; }
			public System.Type Type { get; set; }
		}

		/// <summary>
		/// Keep references to objects around as we can't print them in the same script dump as their declaring type, also avoids processing them multiple times.
		/// string = path to the object's state dump.
		/// </summary>
		internal static Dictionary<ObjectReference, string> ObjectReferences = new Dictionary<ObjectReference, string>();

		/// <summary>
		/// The character used to signal a reference path.
		/// </summary>
		internal static string ReferenceSeperator = "'";
	}
}
