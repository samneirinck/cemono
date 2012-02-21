using System.Reflection;

using System.IO;

using System.Collections;
using System.Collections.Generic;
using System.Linq;

using System.Xml;
using System.Xml.Linq;

namespace CryEngine.Utils
{
	/// <summary>
	/// Handles serializing script instances for an app domain reload.
	/// </summary>
	public static class AppDomainSerializer
	{
		public static void DumpScriptData()
		{
			var xmlSettings = new XmlWriterSettings();
			xmlSettings.Indent = true;

			using (XmlWriter writer = XmlWriter.Create(Path.Combine(PathUtils.GetRootFolder(), "Temp", "MonoScriptData.xml"), xmlSettings))
			{
				writer.WriteStartDocument();
				writer.WriteStartElement("Types");
				
				foreach (var script in ScriptCompiler.CompiledScripts)
				{
					if (script.ScriptInstances != null)
					{
						writer.WriteStartElement("Type");
						writer.WriteAttributeString("Name", script.className);

						foreach (var scriptInstance in script.ScriptInstances)
						{
							System.Type type = scriptInstance.GetType();

							writer.WriteStartElement("Instance");

							// Just a tiiiiiny bit hardcoded.
							int scriptId = System.Convert.ToInt32(type.GetProperty("ScriptId").GetValue(scriptInstance, null));
							writer.WriteAttributeString("Id", scriptId.ToString());

							writer.WriteAttributeString("ReferenceId", ObjectReferences.Count.ToString());
							ObjectReferences.Add(scriptInstance);

							SerializeTypeToXml(scriptInstance, writer);

							writer.WriteEndElement();
						}

						writer.WriteEndElement();
					}
				}

				writer.WriteEndElement();
				writer.WriteEndDocument();
			}
		}

		/// <summary>
		/// Used to keep track of duplicate instances of structs and classes of various kinds.
		/// Saves space in MonoScriptData.xml + solves possible loop issues. - i59
		/// </summary>
		static List<object> ObjectReferences = new List<object>();

		public static void SerializeTypeToXml(object typeInstance, XmlWriter writer)
		{
			System.Type type = typeInstance.GetType();

			while (type != null)
			{
				foreach (var fieldInfo in type.GetFields(BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.DeclaredOnly))
				{
					object value = fieldInfo.GetValue(typeInstance);

					string fieldName = fieldInfo.Name;

					if (ObjectReferences.Contains(value))
					{
						writer.WriteStartElement("Field");
						writer.WriteAttributeString("Name", fieldInfo.Name);
						writer.WriteAttributeString("ReferencesId", ObjectReferences.IndexOf(value).ToString());
						writer.WriteEndElement();
					}
					else if (value != null && !fieldName.Equals("<ScriptId>k__BackingField") && !fieldName.Equals("m_value"))
					{
						string startElement = "Field";
						/*if (fieldName.Contains("k__BackingField"))
						{
							startElement = "Property";

							fieldName = fieldName.Replace("<", "").Replace(">", "").Replace("k__BackingField", "");
						}*/

						writer.WriteStartElement(startElement);

						writer.WriteAttributeString("Name", fieldName);
						writer.WriteAttributeString("Type", fieldInfo.FieldType.Name);
						if (!fieldInfo.FieldType.IsPrimitive && !fieldInfo.FieldType.IsEnum && fieldInfo.FieldType != typeof(string))
						{
							writer.WriteAttributeString("ReferenceId", ObjectReferences.Count.ToString());

							ObjectReferences.Add(value);
						}

						switch (fieldInfo.FieldType.Name)
						{
							case "List`1":
								{
									writer.WriteStartElement("Elements");

									IList list = (IList)value;
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

									IDictionary dictionary = (IDictionary)value;
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
								{
									System.Type valueType = value.GetType();

									bool isString = (valueType == typeof(string));

									if (!valueType.IsEnum && !isString)
										SerializeTypeToXml(value, writer);

									if ((valueType.IsPrimitive || isString || valueType.IsEnum))
										writer.WriteAttributeString("Value", value.ToString());
								}
								break;
						}

						writer.WriteEndElement();
					}
				}

				type = type.BaseType;
			}
		}

		static void AddObjectReference(int desiredIndex, object obj)
		{
			while (ObjectReferences.Count < desiredIndex)
				ObjectReferences.Add(null);

			Console.LogAlways("Adding object reference to index {0}", desiredIndex);

			ObjectReferences.Add(obj);
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
			foreach (var type in scriptData.Descendants("Type"))
			{
				CryScript script = ScriptCompiler.CompiledScripts.Where(Script => Script.className.Equals(type.Attribute("Name").Value)).FirstOrDefault();
				int scriptIndex = ScriptCompiler.CompiledScripts.IndexOf(script);

				if (script != default(CryScript))
				{
					foreach (var instance in type.Elements("Instance"))
					{
						int scriptId = System.Convert.ToInt32(instance.Attribute("Id").Value);

						AddObjectReference(System.Convert.ToInt32(instance.Attribute("ReferenceId").Value), instance);

						if (script.ScriptInstances == null)
							script.ScriptInstances = new List<CryScriptInstance>();

						script.ScriptInstances.Add(System.Activator.CreateInstance(script.Type) as CryScriptInstance);

						if (ScriptCompiler.NextScriptId < scriptId)
							ScriptCompiler.NextScriptId = scriptId;

						script.Type.GetProperty("ScriptId").SetValue(script.ScriptInstances.Last(), scriptId, null);

						ProcessFields(script.ScriptInstances.Last(), instance.Elements("Field"));

						foreach (var property in instance.Elements("Property"))
						{
							PropertyInfo propertyInfo = script.Type.GetProperty(property.Attribute("Name").Value);
							if (propertyInfo != null)
								propertyInfo.SetValue(script.ScriptInstances.Last(), Convert.FromString(property.Attribute("Type").Value, property.Attribute("Value").Value), null);
						}
					}
				}

				ScriptCompiler.CompiledScripts[scriptIndex] = script;
			}
		}

		public static void ProcessFields(object instance, IEnumerable<XElement> fields)
		{
			if (fields == null || fields.Count() < 1)
				return;

			foreach (var field in fields)
			{
				var fieldReferenceAttribute = field.Attribute("ReferencesId");
				if (fieldReferenceAttribute != null)
				{
					FieldInfo fieldInfo = null;
					var baseType = instance.GetType();
					while (fieldInfo == null && baseType != null)
					{
						fieldInfo = baseType.GetField(field.Attribute("Name").Value, BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.DeclaredOnly);

						baseType = baseType.BaseType;
					}

					fieldInfo.SetValue(instance, ObjectReferences.ElementAtOrDefault(System.Convert.ToInt32(fieldReferenceAttribute.Value)));
				}
				else
				{
					FieldInfo fieldInfo = null;
					var baseType = instance.GetType();
					while (fieldInfo == null && baseType != null)
					{
						fieldInfo = baseType.GetField(field.Attribute("Name").Value, BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.DeclaredOnly);

						baseType = baseType.BaseType;
					}

					bool fieldOk = fieldInfo != null;

					var referenceIdAttribute = field.Attribute("ReferenceId");
					if (referenceIdAttribute != null && fieldOk)
						AddObjectReference(System.Convert.ToInt32(referenceIdAttribute.Value), fieldInfo.GetValue(instance));

					if (fieldOk)
					{
						switch (fieldInfo.FieldType.Name)
						{
							case "List`1":
								{
									foreach (var element in field.Elements("Elements").Elements("Element"))
									{
										System.Collections.IList list = (System.Collections.IList)fieldInfo.GetValue(instance);
										list.Add(Convert.FromString(element.Attribute("Type").Value, element.Attribute("Value").Value));

										fieldInfo.SetValue(instance, list);
									}
								}
								break;
							case "Dictionary`2":
								{
									foreach (var element in field.Elements("Elements").Elements("Element"))
									{
										System.Collections.IDictionary dictionary = (System.Collections.IDictionary)fieldInfo.GetValue(instance);
										dictionary.Add(Convert.FromString(element.Attribute("KeyType").Value, element.Attribute("Key").Value), Convert.FromString(element.Attribute("ValueType").Value, element.Attribute("Value").Value));

										fieldInfo.SetValue(instance, dictionary);
									}
								}
								break;
							default:
								{
									var subFields = field.Elements("Field");
									if (subFields.Count() > 0)
									{
										// Limitation; we can only instantiate types with parameterless constructors
										if (fieldInfo.FieldType.GetConstructor(System.Type.EmptyTypes) != null || fieldInfo.FieldType.IsValueType)
										{
											Console.LogAlways("Instantiating {0}", fieldInfo.Name);
											object subFieldInstance = System.Activator.CreateInstance(fieldInfo.FieldType);
											ProcessFields(subFieldInstance, subFields);

											fieldInfo.SetValue(instance, subFieldInstance);
										}
										else
											Console.Log("[Warning] Could not serialize {0} since it did not contain an parameterless constructor", fieldInfo.FieldType.Name);
									}
									else
									{
										if (fieldInfo.FieldType.IsEnum)
											fieldInfo.SetValue(instance, System.Enum.Parse(fieldInfo.FieldType, field.Attribute("Value").Value));
										else
											fieldInfo.SetValue(instance, Convert.FromString(field.Attribute("Type").Value, field.Attribute("Value").Value));
									}
								}
								break;
						}
					}
				}
			}
		}
	}
}
