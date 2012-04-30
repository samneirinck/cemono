using System;
using System.IO;
using System.Reflection;
using System.Runtime.Serialization;

using System.Collections;
using System.Collections.Generic;
using System.Linq;

using CryEngine.Extensions;
using CryEngine.Initialization;

namespace CryEngine.Serialization
{
	public class CrySerializer : IFormatter
	{
		StreamWriter Writer { get; set; }
		StreamReader Reader { get; set; }
		Assembly CallingAssembly { get; set; }
		FormatterConverter Converter { get; set; }

		public CrySerializer()
		{
			Converter = new FormatterConverter();
		}

		public void Serialize(Stream stream, object graph)
		{
			Writer = new StreamWriter(stream);
			Writer.AutoFlush = true;

			StartWrite(new ObjectReference("root", graph));
			stream.Seek(0, SeekOrigin.Begin);
		}

		void WriteLine(object value)
		{
			Writer.WriteLine(value);
		}

		void StartWrite(ObjectReference objectReference)
		{
			if(objectReference.Value == null)
			{
				WriteNull(objectReference);
				return;
			}

			Type valueType = objectReference.Value.GetType();
			if(!IsTypeAllowed(valueType))
				WriteNull(objectReference);
			else if(valueType.IsPrimitive)
			{
				if(valueType != typeof(object) && valueType != typeof(bool) && System.Convert.ToInt32(objectReference.Value) == 0)
					WriteNull(objectReference);
				else
					WriteAny(objectReference);
			}
			else if(valueType == typeof(string))
				WriteString(objectReference);
			else if(valueType.Implements(typeof(IEnumerable)))
			{
				if(valueType.IsGenericType)
					WriteGenericEnumerable(objectReference);
				else
					WriteEnumerable(objectReference);
			}
			else if(valueType.IsEnum)
				WriteEnum(objectReference);
			else if(!valueType.IsPrimitive && !valueType.IsEnum)
				WriteObject(objectReference);
		}

		void WriteNull(ObjectReference objectReference)
		{
			WriteLine("null");
			WriteLine(objectReference.Name);
		}

		void WriteAny(ObjectReference objectReference)
		{
			WriteLine("any");
			WriteLine(objectReference.Name);
			WriteLine(objectReference.Value.GetType().FullName);
			WriteLine(Converter.ToString(objectReference.Value));
		}

		void WriteString(ObjectReference objectReference)
		{
			WriteLine("string");
			WriteLine(objectReference.Name);
			WriteLine(objectReference.Value);
		}

		void WriteEnumerable(ObjectReference objectReference)
		{
			WriteLine("enumerable");
			var array = (objectReference.Value as IEnumerable).Cast<object>();
			WriteLine(array.Count());
			WriteLine(objectReference.Name);
			WriteLine(objectReference.FullName);

			WriteLine(GetIEnumerableElementType(array.GetType()));

			for(int i = 0; i < array.Count(); i++)
				StartWrite(new ObjectReference(i.ToString(), array.ElementAt(i), objectReference));
		}

		void WriteGenericEnumerable(ObjectReference objectReference)
		{
			WriteLine("generic_enumerable");
			var array = (objectReference.Value as IEnumerable).Cast<object>();
			WriteLine(array.Count());
			WriteLine(objectReference.Name);
			WriteLine(objectReference.FullName);

			WriteLine(objectReference.Value.GetType().GetGenericTypeDefinition().FullName);

			var genericArguments = objectReference.Value.GetType().GetGenericArguments();
			WriteLine(genericArguments.Count());
			foreach(var genericArg in genericArguments)
				WriteLine(genericArg.FullName);

			for(int i = 0; i < array.Count(); i++)
				StartWrite(new ObjectReference(i.ToString(), array.ElementAt(i), objectReference));
		}

		void WriteEnum(ObjectReference objectReference)
		{
			WriteLine("enum");
			WriteLine(objectReference.Name);
			WriteLine(objectReference.FullName);
			WriteLine(objectReference.Value.GetType().FullName);
			WriteLine(objectReference.Value);
		}

		void WriteObject(ObjectReference objectReference)
		{
			List<FieldInfo> fields = new List<FieldInfo>();
			var type = objectReference.Value.GetType();
			while(type != null)
			{
				foreach(var field in type.GetFields(BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.DeclaredOnly))
					fields.Add(field);

				type = type.BaseType;
			}

			WriteLine("object");
			WriteLine(fields.Count);
			WriteLine(objectReference.Name);
			WriteLine(objectReference.FullName);
			WriteLine(objectReference.Value.GetType().FullName);

			foreach(var field in fields)
			{
				object fieldValue = field.GetValue(objectReference.Value);

				StartWrite(new ObjectReference(field.Name, fieldValue, objectReference));
			}
		}

		public object Deserialize(Stream stream)
		{
			Reader = new StreamReader(stream);
			CallingAssembly = Assembly.GetCallingAssembly();

			return StartRead().Value;
		}

		string ReadLine()
		{
			return Reader.ReadLine();
		}

		ObjectReference StartRead()
		{
			ObjectReference objReference = new ObjectReference();

			switch(ReadLine())
			{
				case "null": ReadNull(ref objReference); break;
				case "object": ReadObject(ref objReference); break;
				case "generic_enumerable": ReadGenericEnumerable(ref objReference); break;
				case "enumerable": ReadEnumerable(ref objReference); break;
				case "enum": ReadEnum(ref objReference); break;
				case "any": ReadAny(ref objReference); break;
				case "string": ReadString(ref objReference); break;
				default: break;
			}

			return objReference;
		}

		void ReadNull(ref ObjectReference objReference)
		{
			objReference.Name = ReadLine();
		}

		void ReadObject(ref ObjectReference objReference)
		{
			int numFields = int.Parse(ReadLine());
			objReference.Name = ReadLine();
			objReference.FullName = ReadLine();
			string typeName = ReadLine();

			object objectInstance = CreateObjectInstance(typeName);
			for(int i = 0; i < numFields; ++i)
			{
				ObjectReference fieldReference = StartRead();
				FieldInfo fieldInfo = null;

				var type = objectInstance.GetType();
				while(type != null)
				{
					fieldInfo = type.GetField(fieldReference.Name, BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.DeclaredOnly);
					if(fieldInfo != null)
						break;

					type = type.BaseType;
				}

				if(fieldInfo != null)
					fieldInfo.SetValue(objectInstance, fieldReference.Value);
				else
					Debug.LogAlways("failed to find field {0} in type {1}!", fieldReference.Name, typeName);
			}

			objReference.Value = objectInstance;
		}

		void ReadEnumerable(ref ObjectReference objReference)
		{
			var numElements = int.Parse(ReadLine());
			objReference.Name = ReadLine();
			objReference.FullName = ReadLine();
			var typeName = ReadLine();

			var array = Array.CreateInstance(GetType(typeName), numElements);

			for(int i = 0; i != numElements; ++i)
				array.SetValue(StartRead().Value, i);

			objReference.Value = array;
		}

		void ReadGenericEnumerable(ref ObjectReference objReference)
		{
			int elements = int.Parse(ReadLine());
			objReference.Name = ReadLine();
			objReference.FullName = ReadLine();
			string typeName = ReadLine();

			var type = GetType(typeName);

			var numGenericArgs = int.Parse(ReadLine());
			var genericArgs = new Type[numGenericArgs];
			for(int i = 0; i < numGenericArgs; i++)
				genericArgs[i] = GetType(ReadLine());

			var enumerable = CreateGenericObjectInstance(type, genericArgs) as IEnumerable;

			if(enumerable.GetType().Implements(typeof(IDictionary)))
			{
				var dict = enumerable as IDictionary;

				for(int i = 0; i < elements; i++)
				{
					var keyPair = StartRead().Value;
					var valueMethod = keyPair.GetType().GetProperty("Value");
					var keyMethod = keyPair.GetType().GetProperty("Key");

					dict.Add(keyMethod.GetValue(keyPair, null), valueMethod.GetValue(keyPair, null));
				}

				enumerable = dict;
			}
			else
			{
				var list = enumerable as IList;

				for(int i = 0; i < elements; i++)
					list.Add(StartRead().Value);

				enumerable = list;
			}

			objReference.Value = enumerable;
		}

		void ReadAny(ref ObjectReference objReference)
		{
			objReference.Name = ReadLine();
			string typeName = ReadLine();
			string valueString = ReadLine();

			object value = null;
			var type = GetType(typeName);
			if(type != null)
				value = Converter.Convert(valueString, type);

			objReference.Value = value;
		}

		void ReadString(ref ObjectReference objReference)
		{
			objReference.Name = ReadLine();
			objReference.Value = ReadLine();
		}

		void ReadEnum(ref ObjectReference objReference)
		{
			objReference.Name = ReadLine();
			objReference.FullName = ReadLine();
			string typeName = ReadLine();
			string valueString = ReadLine();

			object value = null;
			var type = GetType(typeName);
			if(type != null)
				value = Enum.Parse(type, valueString);
			else
				Debug.LogAlways("Failed to get type {0}", typeName);

			objReference.Value = value;
		}

		static System.Type[] forbiddenTypes = new System.Type[] { typeof(MethodInfo) };

		bool IsTypeAllowed(System.Type type)
		{
			foreach(var forbiddenType in forbiddenTypes)
			{
				if(type == forbiddenType || (type.HasElementType && type.GetElementType() == forbiddenType))
					return false;

				if(type.Implements(forbiddenType))
					return false;
			}

			return true;
		}

		Type GetType(string typeName)
		{
			if(typeName.Contains('+'))
			{
				var splitString = typeName.Split('+');
				var ownerType = GetType(splitString.First());

				return ownerType.Assembly.GetType(typeName);
			}

			Type type = Type.GetType(typeName);
			if(type != null)
				return type;

			foreach(var assembly in AppDomain.CurrentDomain.GetAssemblies())
			{
				type = assembly.GetType(typeName);
				if(type != null)
					return type;
			}

			if(type == null)
				throw new Exception(string.Format("Could not localize type with name {0}", typeName));

			return type;
		}

		Type GetIEnumerableElementType(Type enumerableType)
		{
			Type type = enumerableType.GetElementType();
			if(type != null)
				return type;


			// Not an array type, we've got to use an alternate method to get the type of elements contained within.
			if(enumerableType.IsGenericType && enumerableType.GetGenericTypeDefinition() == typeof(IEnumerable<>))
				type = enumerableType.GetGenericArguments()[0];
			else
			{
				Type[] interfaces = enumerableType.GetInterfaces();
				foreach(Type i in interfaces)
					if(i.IsGenericType && i.GetGenericTypeDefinition().Equals(typeof(IEnumerable<>)))
						type = i.GetGenericArguments()[0];
			}

			return type;
		}

		object CreateGenericObjectInstance(Type type, params Type[] genericArguments)
		{
			Type genericType = type.MakeGenericType(genericArguments);

			return System.Activator.CreateInstance(genericType);
		}

		object CreateObjectInstance(string typeName)
		{
			Type type = GetType(typeName);

			if(type.GetConstructor(System.Type.EmptyTypes) != null || type.IsValueType)
				return System.Activator.CreateInstance(type);

			throw new Exception(string.Format("Could not serialize type {0} since it did not containg a parameterless constructor", type.Name));
		}

		public SerializationBinder Binder { get { return null; } set { } }
		public ISurrogateSelector SurrogateSelector { get { return null; } set { } }
		public StreamingContext Context { get { return new StreamingContext(StreamingContextStates.Persistence); } set { } }
	}
}