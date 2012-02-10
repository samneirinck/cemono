using System;
using System.Linq;
using System.IO;
using System.Collections.Generic;

using System.Reflection;

using CryEngine.Extensions;

using System.ComponentModel;
using System.Threading.Tasks;

using System.Xml;
using System.Xml.Linq;

namespace CryEngine
{
	/// <summary>
	/// The script compiler is responsible for all CryScript compilation.
	/// </summary>
	public class ScriptCompiler : _ScriptCompiler
	{
		public ScriptCompiler()
		{
#if !RELEASE
			Pdb2Mdb.Driver.Convert(Assembly.GetExecutingAssembly());
#endif

			compiledScripts = new List<CryScript>();
			flowNodes = new List<StoredNode>();
			referencedAssemblies = new List<string>();

			nextScriptId = 0;
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

			compiledScripts.AddRange(scripts);
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
			int index = compiledScripts.FindIndex(x => x.className.Equals(scriptName));
			if (index == -1)
			{
				Console.LogAlways("Failed to instantiate {0}, compiled script could not be found.", scriptName);

				return null;
			}

			CryScript script = compiledScripts.ElementAt(index);
			if (!script.Type.Implements(typeof(CryScriptInstance)))
				return null;

			nextScriptId++;

			if (script.ScriptInstances == null)
				script.ScriptInstances = new List<CryScriptInstance>();

			script.ScriptInstances.Add(Activator.CreateInstance(script.Type, constructorParams) as CryScriptInstance);
			script.ScriptInstances.Last().ScriptId = nextScriptId;

			compiledScripts[index] = script;

			return script.ScriptInstances.Last();
		}

		[EditorBrowsable(EditorBrowsableState.Never)]
		public void RemoveInstance(int scriptId, string scriptName)
		{
			int index = compiledScripts.FindIndex(x => x.className.Equals(scriptName));
			if (index == -1)
				return;

			CryScript script = compiledScripts[index];

			if (script.ScriptInstances != null)
			{
				int instanceIndex = script.ScriptInstances.FindIndex(x => x.ScriptId == scriptId);
				if (instanceIndex == -1)
				{
					Console.LogAlways("Failed to remove script of type {0} with id {1}; instance was not found.", scriptName, scriptId);
					return;
				}

				script.ScriptInstances.RemoveAt(instanceIndex);

				compiledScripts[index] = script;
			}
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
			for (int i = 0; i < compiledScripts.Count; i++)
			{
				if (compiledScripts[i].ScriptInstances != null)
				{
					CryScriptInstance tempInstance = compiledScripts[i].ScriptInstances.FirstOrDefault(instance => instance.ScriptId == id);

					if (tempInstance != default(CryScriptInstance))
						return tempInstance;
				}
			}

			return null;
		}

		public void DumpScriptData()
		{
			var xmlSettings = new XmlWriterSettings();
			xmlSettings.Indent = true;

			using (XmlWriter writer = XmlWriter.Create(Path.Combine(PathUtils.GetRootFolder(), "Temp", "MonoScriptData.xml"), xmlSettings))
			{
				writer.WriteStartDocument();
				writer.WriteStartElement("Types");

				foreach (var script in compiledScripts)
				{
					if(script.ScriptInstances!=null)
					{
						writer.WriteStartElement("Type");
						writer.WriteAttributeString("Name", script.className);

						foreach (var scriptInstance in script.ScriptInstances)
						{
							Type type = scriptInstance.GetType();

							writer.WriteStartElement("Instance");

							// Just a tiiiiiny bit hardcoded.
							string scriptField = "<ScriptId>k__BackingField";
							int scriptId = Convert.ToInt32(type.GetProperty("ScriptId").GetValue(scriptInstance, null));
							writer.WriteAttributeString("Id", scriptId.ToString());
							
							while (type != null)
							{
								foreach (var fieldInfo in type.GetFields(BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.DeclaredOnly))
								{
									object value = fieldInfo.GetValue(scriptInstance);
									string fieldName = fieldInfo.Name;

									if (value != null && !fieldName.Equals(scriptField))
									{
										if (fieldName.Contains("k__BackingField"))
										{
											writer.WriteStartElement("Property");

											fieldName = fieldName.Replace("<", "").Replace(">", "").Replace("k__BackingField", "");
										}
										else
											writer.WriteStartElement("Field");

										writer.WriteAttributeString("Name", fieldName);
										writer.WriteAttributeString("Type", fieldInfo.FieldType.Name);
										writer.WriteAttributeString("Value", fieldInfo.GetValue(scriptInstance).ToString());
										writer.WriteEndElement();
									}
								}

								type = type.BaseType;
							}

							writer.WriteEndElement();
						}

						writer.WriteEndElement();
					}
				}

				writer.WriteEndElement();
				writer.WriteEndDocument();
			}
		}

		public object StringToValue(string type, string value)
		{
			switch (type)
			{
				case "Boolean":
					return Convert.ToBoolean(value);
				case "UInt32":
					return Convert.ToUInt32(value);
				case "Int32":
					return Convert.ToInt32(value);
			}

			return null;
		}

		public void TrySetScriptData()
		{
			string filePath = Path.Combine(PathUtils.GetRootFolder(), "Temp", "MonoScriptData.xml");
			if (!File.Exists(filePath))
			{
				Console.LogAlways("Failed to retrieve serialized MonoScriptData");
				return;
			}

			XDocument scriptData = XDocument.Load(filePath);
			foreach(var type in scriptData.Descendants("Type"))
			{
				CryScript script = compiledScripts.Where(Script => Script.className.Equals(type.Attribute("Name").Value)).FirstOrDefault();
				if (script != default(CryScript))
				{
					foreach (var instance in type.Elements("Instance"))
					{
						int scriptId = Convert.ToInt32(instance.Attribute("Id").Value);

						if (script.ScriptInstances == null)
							script.ScriptInstances = new List<CryScriptInstance>();

						script.ScriptInstances.Add(Activator.CreateInstance(script.Type) as CryScriptInstance);

						if (nextScriptId < scriptId)
							nextScriptId = scriptId;

						script.Type.GetProperty("ScriptId").SetValue(script.ScriptInstances.Last(), scriptId, null);

						foreach (var field in instance.Elements("Field"))
						{
							FieldInfo fieldInfo = script.Type.GetField(field.Attribute("Name").Value);
							if (fieldInfo != null)// && !fieldInfo.FieldType.Name.Equals("Dictionary`2") && !fieldInfo.FieldType.Name.Equals("List`1"))
								fieldInfo.SetValue(script.ScriptInstances.Last(), StringToValue(field.Attribute("Type").Value, field.Attribute("Value").Value));
						}

						foreach (var property in instance.Elements("Property"))
						{
							PropertyInfo propertyInfo = script.Type.GetProperty(property.Attribute("Name").Value);
							if (propertyInfo != null)
								propertyInfo.SetValue(script.ScriptInstances.Last(), StringToValue(property.Attribute("Type").Value, property.Attribute("Value").Value), null);
						}
					}
				}
			}
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

		/// <summary>
		/// Called once per frame.
		/// </summary>
		public void OnUpdate(float frameTime)
		{
			Time.DeltaTime = frameTime;

			Parallel.ForEach(compiledScripts, script =>
			{
				if(script.ScriptInstances!=null)
					script.ScriptInstances.Where(i => i.ReceiveUpdates).ToList().ForEach(i => i.OnUpdate());
			});
		}

		List<Scriptbind> scriptBinds;

		List<CryScript> compiledScripts;
		int nextScriptId;
	}

	public struct ScriptState
	{
		public ScriptState(string TypeName, FieldData[] Fields)
			: this()
		{
			fields = Fields;
			typeName = TypeName;
		}

		public FieldData[] fields;
		public string typeName;

		public struct FieldData
		{
			public FieldData(string name, object val)
				: this()
			{
				fieldName = name;
				value = val;
			}

			public string fieldName;
			public object value;
		}
	}
}
