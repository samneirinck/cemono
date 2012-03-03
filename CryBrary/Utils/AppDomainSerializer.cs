using System.Reflection;

using System.IO;

using System.Collections;
using System.Collections.ObjectModel;
using System.Collections.Generic;

using System.Linq;

using System.Xml;
using System.Xml.Linq;

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
			var xmlSettings = new XmlWriterSettings();
			xmlSettings.Indent = true;

			using (XmlWriter writer = XmlWriter.Create(Path.Combine(PathUtils.GetRootFolder(), "Temp", "MonoScriptData.xml"), xmlSettings))
			{
				writer.WriteStartDocument();

				writer.WriteStartElement("ScriptData");

				{
					writer.WriteStartElement("Types");

					foreach (var script in ScriptCompiler.CompiledScripts)
					{
						if (script.ScriptInstances != null)
						{
							writer.WriteStartElement("Type");
							writer.WriteAttributeString("Name", script.ClassName);

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
				}

				{
					writer.WriteStartElement("Subsystems");

					WriteSubsystem(typeof(EntitySystem), writer);
					//WriteSubsystem(typeof(InputSystem), writer);
					WriteSubsystem(typeof(GameRules), writer);
					WriteSubsystem(typeof(Debug), writer);

					writer.WriteEndElement();
				}

				writer.WriteEndElement();

				writer.WriteEndDocument();
			}
		}

		static void WriteSubsystem(System.Type type, XmlWriter writer)
		{
			var fields = type.GetFields(BindingFlags.Static | BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.DeclaredOnly);
			if (fields.Length > 0)
			{
				writer.WriteStartElement("System");
				writer.WriteAttributeString("Name", type.FullName);

				foreach (var fieldInfo in fields)
					WriteField(fieldInfo.GetValue(null), fieldInfo, writer);

				writer.WriteEndElement();
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
					WriteField(fieldInfo.GetValue(typeInstance), fieldInfo, writer);

				type = type.BaseType;
			}
		}

		static void WriteField(object value, FieldInfo fieldInfo, XmlWriter writer)
		{
			string fieldName = fieldInfo.Name;

			if (value == null)
				return;

			var valueType = value.GetType();

			bool referenceExists = ObjectReferences.Contains(value);
			if (referenceExists)
			{
				writer.WriteStartElement("Field");
				writer.WriteAttributeString("Name", fieldInfo.Name);
				writer.WriteAttributeString("ReferencesId", ObjectReferences.IndexOf(value).ToString());
				writer.WriteEndElement();
			}
			else if (!fieldName.Equals("<ScriptId>k__BackingField"))
			{
				if (fieldInfo.FieldType.Implements(typeof(IList)))
				{
					IList list = (IList)value;
					if (list.Count <= 0)
						return;

					writer.WriteStartElement("Field");

					writer.WriteAttributeString("Name", fieldInfo.Name);
					writer.WriteAttributeString("Type", valueType.Name);

					if (!referenceExists && !valueType.IsPrimitive && !valueType.IsEnum && valueType != typeof(string))
					{
						writer.WriteAttributeString("ReferenceId", ObjectReferences.Count.ToString());

						ObjectReferences.Add(value);
					}

					writer.WriteStartElement("Elements");

					foreach (var listObject in list)
					{
						writer.WriteStartElement("Element");
						WriteValue(listObject, writer);// writer.WriteAttributeString("Value", listObject.ToString());
						writer.WriteEndElement();
					}

					writer.WriteEndElement();
				}
				else if (fieldInfo.FieldType.Implements(typeof(IDictionary)))
				{
					IDictionary dictionary = (IDictionary)value;
					if (dictionary.Count <= 0)
						return;

					writer.WriteStartElement("Field");

					writer.WriteAttributeString("Name", fieldInfo.Name);
					writer.WriteAttributeString("Type", valueType.Name);

					if (!referenceExists && !valueType.IsPrimitive && !valueType.IsEnum && valueType != typeof(string))
					{
						writer.WriteAttributeString("ReferenceId", ObjectReferences.Count.ToString());

						ObjectReferences.Add(value);
					}

					writer.WriteStartElement("Elements");

					foreach (var key in dictionary.Keys)
					{
						writer.WriteStartElement("Element");
						writer.WriteStartElement("Key");
						WriteValue(key, writer);
						writer.WriteEndElement();
						writer.WriteStartElement("Value");
						WriteValue(dictionary[key], writer);
						writer.WriteEndElement();
						writer.WriteEndElement();
					}

					writer.WriteEndElement();
				}
				else
				{
					writer.WriteStartElement("Field");
					WriteValue(value, writer, fieldName);
				}

				writer.WriteEndElement(); // "Field"
			}
		}

		static void WriteValue(object value, XmlWriter writer, string name = null)
		{
			if (!string.IsNullOrEmpty(name))
				writer.WriteAttributeString("Name", name);

			var valueType = value.GetType();

			writer.WriteAttributeString("Type", valueType.Name);

			bool isString = (valueType == typeof(string));

			if (!valueType.IsPrimitive && !valueType.IsEnum && !isString)
			{
				writer.WriteAttributeString("ReferenceId", ObjectReferences.Count.ToString());
				ObjectReferences.Add(value);

				SerializeTypeToXml(value, writer);
			}
			else
				writer.WriteAttributeString("Value", value.ToString());
		}

		static void AddObjectReference(int desiredIndex, object obj)
		{
			while (ObjectReferences.Count < desiredIndex)
				ObjectReferences.Add(null);

			ObjectReferences.Add(obj);
		}

		public static void TrySetScriptData()
		{
			string filePath = Path.Combine(PathUtils.GetRootFolder(), "Temp", "MonoScriptData.xml");
			if (!File.Exists(filePath))
				return;

			XDocument scriptData = XDocument.Load(filePath);

			var scriptDataElement = scriptData.Element("ScriptData");
			foreach (var type in scriptDataElement.Element("Types").Elements("Type"))
			{
				CryScript script = ScriptCompiler.CompiledScripts.Where(Script => Script.ClassName.Equals(type.Attribute("Name").Value)).FirstOrDefault();

				if (script != default(CryScript))
				{
					foreach (var instanceElement in type.Elements("Instance"))
					{
						int scriptId = System.Convert.ToInt32(instanceElement.Attribute("Id").Value);

						if(script.ScriptInstances == null)
							script.ScriptInstances = new Collection<CryScriptInstance>();

						script.ScriptInstances.Add(System.Activator.CreateInstance(script.ClassType) as CryScriptInstance);

						AddObjectReference(System.Convert.ToInt32(instanceElement.Attribute("ReferenceId").Value), script.ScriptInstances.Last());

						if (ScriptCompiler.NextScriptId < scriptId)
							ScriptCompiler.NextScriptId = scriptId;

						script.ClassType.GetProperty("ScriptId").SetValue(script.ScriptInstances.Last(), scriptId, null);

						ProcessFields(script.ScriptInstances.Last(), instanceElement.Elements("Field"));
					}
				}

				int scriptIndex = ScriptCompiler.CompiledScripts.IndexOf(script);
				ScriptCompiler.CompiledScripts[scriptIndex] = script;
			}

			foreach (var subSystem in scriptDataElement.Elements("Subsystems").Elements("System"))
			{
				var type = System.Type.GetType(subSystem.Attribute("Name").Value);

				ProcessFields(null, subSystem.Elements("Field"), type);
			}

			foreach (var scriptInstance in ReloadedScriptInstances)
				scriptInstance.OnPostScriptReload();

			ReloadedScriptInstances = null;

			File.Delete(filePath);
		}

		public static void ProcessFields(object instance, IEnumerable<XElement> fields, System.Type type = null /* used for static types */)
		{
			if (fields == null || fields.Count() < 1)
				return;

			if (instance as CryScriptInstance != null)
				ReloadedScriptInstances.Add(instance as CryScriptInstance);

			var instanceType = type != null ? type : instance.GetType();

			foreach (var field in fields)
			{
				var fieldReferenceAttribute = field.Attribute("ReferencesId");
				if (fieldReferenceAttribute != null)
				{
					FieldInfo fieldInfo = null;
					var baseType = instanceType;
					while (fieldInfo == null && baseType != null)
					{
						fieldInfo = baseType.GetField(field.Attribute("Name").Value, instance == null && type != null ? BindingFlags.Static : BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.DeclaredOnly);

						baseType = baseType.BaseType;
					}

					fieldInfo.SetValue(instance, ObjectReferences.ElementAtOrDefault(System.Convert.ToInt32(fieldReferenceAttribute.Value)));
				}
				else// if(!field.Attribute("Name").Value.Equals("inputMethods")) // this needs to be solved asap
				{
					FieldInfo fieldInfo = null;
					var baseType = instanceType;
					while (fieldInfo == null && baseType != null)
					{
						fieldInfo = baseType.GetField(field.Attribute("Name").Value, instance == null && type != null ? BindingFlags.Static : BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.DeclaredOnly);

						baseType = baseType.BaseType;
					}

					bool fieldOk = fieldInfo != null;

					if (fieldOk)
					{
						if (fieldInfo.FieldType.Implements(typeof(IList)))
						{
							var elements = field.Element("Elements");
							if (elements != null)
							{
								System.Collections.IList list = (System.Collections.IList)fieldInfo.GetValue(instance);

								foreach (var element in elements.Elements("Element"))
									list.Add(ConvertTypeValue(element.Attribute("Type").Value, element.Attribute("Value").Value, null));

								fieldInfo.SetValue(instance, list);
							}
						}
						else if (fieldInfo.FieldType.Implements(typeof(IDictionary)))
						{
							var elements = field.Element("Elements");
							if (elements != null)
							{
								System.Collections.IDictionary dictionary = (System.Collections.IDictionary)fieldInfo.GetValue(instance);

								foreach (var element in elements.Elements("Element"))
								{
									var key = element.Element("Key");
									var value = element.Element("Value");

									dictionary.Add(ConvertTypeValue(key.Attribute("Type").Value, key.Attribute("Value").Value, null), ConvertTypeValue(value.Attribute("Type").Value, value.Attribute("Value").Value, null));
								}

								fieldInfo.SetValue(instance, dictionary);
							}
						}
						else
						{
							var subFields = field.Elements("Field");
							if (subFields.Count() > 0)
							{
								// Limitation; we can only instantiate types with parameterless constructors
								if (fieldInfo.FieldType.GetConstructor(System.Type.EmptyTypes) != null || fieldInfo.FieldType.IsValueType)
								{
									object subFieldInstance = System.Activator.CreateInstance(fieldInfo.FieldType);

									ProcessFields(subFieldInstance, subFields);

									fieldInfo.SetValue(instance, subFieldInstance);
								}
								else
									Debug.Log("[Warning] Could not serialize {0} since it did not contain an parameterless constructor", fieldInfo.FieldType.Name);
							}
							else
							{
								if (fieldInfo.FieldType.IsEnum)
									fieldInfo.SetValue(instance, System.Enum.Parse(fieldInfo.FieldType, field.Attribute("Value").Value));
								else
									fieldInfo.SetValue(instance, Convert.FromString(field.Attribute("Type").Value, field.Attribute("Value").Value));
							}
						}
					}

					var referenceIdAttribute = field.Attribute("ReferenceId");
					if (referenceIdAttribute != null && fieldOk)
						AddObjectReference(System.Convert.ToInt32(referenceIdAttribute.Value), fieldInfo.GetValue(instance));
				}
			}
		}

		static List<CryScriptInstance> ReloadedScriptInstances = new List<CryScriptInstance>();

		static object ConvertTypeValue(string type, string value, object parent)
		{
			return Convert.FromString(type, value);
		}
	}
}
