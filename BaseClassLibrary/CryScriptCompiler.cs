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
	public static partial class ScriptCompiler
	{
		public static void Initialize()
		{
#if !RELEASE
			//Pdb2Mdb.Driver.Convert(Assembly.GetExecutingAssembly());
#endif

			compiledScripts = new List<CryScript>();
			flowNodes = new List<StoredNode>();
			referencedAssemblies = new List<string>();

			nextScriptId = 0;

			//GenerateScriptbindAssembly(scriptBinds.ToArray());

			referencedAssemblies.AddRange(System.AppDomain.CurrentDomain.GetAssemblies().Select(a => a.Location).ToArray());

			LoadPrecompiledAssemblies();

			AddScripts(CompileScriptsInFolders(
				PathUtils.GetScriptFolder(MonoScriptType.Entity),
				PathUtils.GetScriptFolder(MonoScriptType.GameRules),
				PathUtils.GetScriptFolder(MonoScriptType.FlowNode)
			));
		}

		public static void PostInit()
		{
			// These have to be registered later on due to the flow system being initialized late.
			RegisterFlownodes();
		}

		private static void LoadPrecompiledAssemblies()
		{
			//Add pre-compiled assemblies / plugins
			AddScripts(ScriptCompiler.LoadLibrariesInFolder(Path.Combine(PathUtils.GetScriptsFolder(), "Plugins")));
		}

		private static void AddScripts(CryScript[] scripts)
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
		public static CryScriptInstance InstantiateScript(string scriptName, object[] constructorParams = null)
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
			//if (!script.Type.Implements(typeof(CryScriptInstance)))
			//	return null;

			nextScriptId++;

			if (script.ScriptInstances == null)
				script.ScriptInstances = new List<CryScriptInstance>();

			script.ScriptInstances.Add(System.Activator.CreateInstance(script.Type, constructorParams) as CryScriptInstance);
			script.ScriptInstances.Last().ScriptId = nextScriptId;

			compiledScripts[index] = script;

			return script.ScriptInstances.Last();
		}

		/// <summary>
		/// Adds an script instance to the script collection and returns its new id.
		/// </summary>
		/// <param name="instance"></param>
		public static int AddScriptInstance(CryScriptInstance instance)
		{
			int scriptIndex = compiledScripts.FindIndex(x => x.className == instance.GetType().Name);
			if (scriptIndex != -1)
			{
				CryScript script = compiledScripts.ElementAt(scriptIndex);

				if (script.ScriptInstances == null)
					script.ScriptInstances = new List<CryScriptInstance>();
				else if (script.ScriptInstances.Contains(instance))
					return -1;

				nextScriptId++;

				instance.ScriptId = nextScriptId;
				script.ScriptInstances.Add(instance);

				compiledScripts[scriptIndex] = script;

				return nextScriptId;
			}

			Console.LogAlways("Couldn't add script {0}", instance.GetType().Name);
			return -1;
		}

		/// <summary>
		/// Locates and destructs the script with the assigned scriptId.
		/// 
		/// Warning: Not supplying the scriptName will make the method execute slower.
		/// </summary>
		/// <param name="scriptId"></param>
		/// <param name="scriptName"></param>
		public static void RemoveInstance(int scriptId, string scriptName = "")
		{
			Console.LogAlways("RemoveInstance {0}", scriptId);

			if (scriptName.Length > 0)
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
			else
			{
				for (int i = 0; i < compiledScripts.Count; i++)
				{
					CryScript script = compiledScripts[i];

					if (script.ScriptInstances != null)
					{
						int scriptIndex = script.ScriptInstances.FindIndex(x => x.ScriptId == scriptId);
						if (scriptIndex != -1)
						{
							script.ScriptInstances.RemoveAt(scriptIndex);

							compiledScripts[i] = script;

							break;
						}
					}
				}
			}
		}

		[EditorBrowsable(EditorBrowsableState.Never)]
		public static object InvokeScriptFunctionById(int id, string func, object[] args = null)
		{
			CryScriptInstance scriptInstance = GetScriptInstanceById(id);
			if (scriptInstance == default(CryScriptInstance))
			{
				Console.LogAlways("Failed to invoke method, script instance was invalid");
				return null;
			}

			return InvokeScriptFunction(scriptInstance, func, args);
		}

		public static CryScriptInstance GetScriptInstanceById(int id)
		{
			var scripts = compiledScripts.Where(script => script.ScriptInstances != null);

			CryScriptInstance scriptInstance = null;
			foreach (var script in scripts)
			{
				scriptInstance = script.ScriptInstances.FirstOrDefault(instance => instance.ScriptId == id);

				if (scriptInstance != default(CryScriptInstance))
					return scriptInstance;
			}

			return null;
		}

		public static int GetEntityScriptId(uint entityId, System.Type scriptType = null)
		{
			var scripts = compiledScripts.Where(script => (scriptType != null ? script.Type.Implements(scriptType) : true) && script.ScriptInstances != null);

			foreach (var compiledScript in scripts)
			{
				foreach (var script in compiledScript.ScriptInstances)
				{
					var scriptEntity = script as StaticEntity;
					if (scriptEntity != null && scriptEntity.Id == entityId)
						return script.ScriptId;
				}
			}

			return -1;
		}

		public static void DumpScriptData()
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
							System.Type type = scriptInstance.GetType();

							writer.WriteStartElement("Instance");

							// Just a tiiiiiny bit hardcoded.
							string scriptField = "<ScriptId>k__BackingField";
							int scriptId = System.Convert.ToInt32(type.GetProperty("ScriptId").GetValue(scriptInstance, null));
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

										switch(fieldInfo.FieldType.Name)
										{
											case "List`1":
												{
													writer.WriteStartElement("Elements");

													System.Collections.IList list = (System.Collections.IList)value;
													foreach (var listObject in list)
													{
														writer.WriteStartElement("Element");
														writer.WriteAttributeString("Type", listObject.GetType().Name);
														writer.WriteAttributeString("Value", listObject.ToString());
														writer.WriteEndElement();
													}

													writer.WriteEndElement();
												}
												break;
											case "Dictionary`2":
												{
													writer.WriteStartElement("Elements");

													System.Collections.IDictionary dictionary = (System.Collections.IDictionary)value;
													foreach (var key in dictionary.Keys)
													{
														writer.WriteStartElement("Element");
														writer.WriteAttributeString("KeyType", key.GetType().Name);
														writer.WriteAttributeString("Key", key.ToString());
														writer.WriteAttributeString("ValueType", dictionary[key].GetType().Name);
														writer.WriteAttributeString("Value", dictionary[key].ToString());
														writer.WriteEndElement();
													}

													writer.WriteEndElement();
												}
												break;
											default:
												writer.WriteAttributeString("Value", value.ToString());
												break;
										}

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

		public static void TrySetScriptData()
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
				int scriptIndex = compiledScripts.IndexOf(script);

				if (script != default(CryScript))
				{
					foreach (var instance in type.Elements("Instance"))
					{
						int scriptId = System.Convert.ToInt32(instance.Attribute("Id").Value);

						if (script.ScriptInstances == null)
							script.ScriptInstances = new List<CryScriptInstance>();

						script.ScriptInstances.Add(System.Activator.CreateInstance(script.Type) as CryScriptInstance);

						if (nextScriptId < scriptId)
							nextScriptId = scriptId;

						script.Type.GetProperty("ScriptId").SetValue(script.ScriptInstances.Last(), scriptId, null);

						foreach (var field in instance.Elements("Field"))
						{
							FieldInfo fieldInfo = script.Type.GetField(field.Attribute("Name").Value);
							if (fieldInfo != null)// && !fieldInfo.FieldType.Name.Equals("Dictionary`2") && !fieldInfo.FieldType.Name.Equals("List`1"))
							{
								switch(fieldInfo.FieldType.Name)
								{
									case "List`1":
										{
											foreach (var element in field.Elements("Elements").Elements("Element"))
											{
												System.Collections.IList list = (System.Collections.IList)fieldInfo.GetValue(script.ScriptInstances.Last());
												list.Add(Convert.FromString(element.Attribute("Type").Value, element.Attribute("Value").Value));

												fieldInfo.SetValue(script.ScriptInstances.Last(), list);
											}
										}
										break;
									case "Dictionary`2":
										{
											foreach (var element in field.Elements("Elements").Elements("Element"))
											{
												System.Collections.IDictionary dictionary = (System.Collections.IDictionary)fieldInfo.GetValue(script.ScriptInstances.Last());
												dictionary.Add(Convert.FromString(element.Attribute("KeyType").Value, element.Attribute("Key").Value), Convert.FromString(element.Attribute("ValueType").Value, element.Attribute("Value").Value));

												fieldInfo.SetValue(script.ScriptInstances.Last(), dictionary);
											}
										}
										break;
									default:
										fieldInfo.SetValue(script.ScriptInstances.Last(), Convert.FromString(field.Attribute("Type").Value, field.Attribute("Value").Value));
										break;
								}
							}
						}

						foreach (var property in instance.Elements("Property"))
						{
							PropertyInfo propertyInfo = script.Type.GetProperty(property.Attribute("Name").Value);
							if (propertyInfo != null)
								propertyInfo.SetValue(script.ScriptInstances.Last(), Convert.FromString(property.Attribute("Type").Value, property.Attribute("Value").Value), null);
						}
					}
				}

				compiledScripts[scriptIndex] = script;
			}
		}

		/// <summary>
		/// Automagically registers scriptbind methods to rid us of having to add them in both C# and C++.
		/// </summary>
		[EditorBrowsable(EditorBrowsableState.Never)]
		public static void RegisterScriptbind(string namespaceName, string className, object[] methods)
		{
			if (scriptBinds == null)
				scriptBinds = new List<Scriptbind>();

			scriptBinds.Add(new Scriptbind(namespaceName, className, methods));
		}

		/// <summary>
		/// Called once per frame.
		/// </summary>
		public static void OnUpdate(float frameTime)
		{
			Time.DeltaTime = frameTime;

			Parallel.ForEach(compiledScripts, script =>
			{
				if(script.ScriptInstances!=null)
					script.ScriptInstances.Where(i => i.ReceiveUpdates).ToList().ForEach(i => i.OnUpdate());
			});
		}

		static List<Scriptbind> scriptBinds;

		static List<CryScript> compiledScripts;
		static int nextScriptId;
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
