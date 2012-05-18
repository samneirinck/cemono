using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Runtime.Serialization;
using CryEngine.Extensions;

namespace CryEngine.Serialization
{
	public class CrySerializer : IFormatter
	{
		StreamWriter Writer { get; set; }
		StreamReader Reader { get; set; }
		FormatterConverter Converter { get; set; }

		/// <summary>
		/// We store a dictionary of all serialized objects in order to not create new instances of types with identical hash codes. (same objects)
		/// </summary>
		Dictionary<int, ObjectReference> ObjectReferences { get; set; }

		public CrySerializer()
		{
			Converter = new FormatterConverter();
			ObjectReferences = new Dictionary<int, ObjectReference>();
		}

		public void Serialize(Stream stream, object graph)
		{
			if(stream == null)
				throw new ArgumentNullException("stream");
			else if(graph == null)
				throw new ArgumentNullException("graph");

			Writer = new StreamWriter(stream);
			Writer.AutoFlush = true;

			ObjectReferences.Clear();
			CurrentLine = 0;

			StartWrite(new ObjectReference("root", graph));
			stream.Seek(0, SeekOrigin.Begin);
		}

		void WriteLine(object value)
		{
			CurrentLine++;
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

			if(valueType.IsPrimitive)
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
			else
			{
				if(objectReference.Value is Type)
					WriteType(objectReference);
				else if(valueType.Implements(typeof(MemberInfo)))
					WriteMemberInfo(objectReference);
				else
					WriteObject(objectReference);
			}
		}

		void WriteNull(ObjectReference objectReference)
		{
			WriteLine("null");
			WriteLine(objectReference.Name);
		}

		void WriteReference(ObjectReference objReference, int line)
		{
			WriteLine("reference");
			WriteLine(objReference.Name);
			WriteLine(line);
		}

		void WriteAny(ObjectReference objectReference)
		{
			WriteLine("any");
			WriteLine(objectReference.Name);
			WriteType(objectReference.Value.GetType());
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
			if(TryWriteReference(objectReference))
				return;

			WriteLine("enumerable");
			var array = (objectReference.Value as IEnumerable).Cast<object>();
			WriteLine(array.Count());
			WriteLine(objectReference.Name);

			WriteType(GetIEnumerableElementType(array.GetType()));

			for(int i = 0; i < array.Count(); i++)
				StartWrite(new ObjectReference(i.ToString(), array.ElementAt(i)));
		}

		void WriteGenericEnumerable(ObjectReference objectReference)
		{
			if(TryWriteReference(objectReference))
				return;

			WriteLine("generic_enumerable");
			var array = (objectReference.Value as IEnumerable).Cast<object>();
			WriteLine(array.Count());
			WriteLine(objectReference.Name);

			WriteType(objectReference.Value.GetType());

			for(int i = 0; i < array.Count(); i++)
				StartWrite(new ObjectReference(i.ToString(), array.ElementAt(i)));
		}

		void WriteEnum(ObjectReference objectReference)
		{
			WriteLine("enum");
			WriteLine(objectReference.Name);
			WriteType(objectReference.Value.GetType());
			WriteLine(objectReference.Value);
		}

		void WriteObject(ObjectReference objectReference)
		{
			if(TryWriteReference(objectReference))
				return;

			var type = objectReference.Value.GetType();

			WriteLine("object");
			WriteLine(objectReference.Name);
			WriteType(type);

			var fields = type.GetFields(BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic);
			WriteLine(fields.Length);
			foreach(var field in fields)
			{
				object fieldValue = field.GetValue(objectReference.Value);

				StartWrite(new ObjectReference(field.Name, fieldValue));
			}
		}

		void WriteMemberInfo(ObjectReference objectReference)
		{
			if(TryWriteReference(objectReference))
				return;

			WriteLine("memberinfo");
			WriteLine(objectReference.Name);

			var memberInfo = objectReference.Value as MemberInfo;
			WriteLine(memberInfo.Name);
			WriteType(memberInfo.DeclaringType);
			WriteLine(memberInfo.MemberType);
		}

		void WriteType(ObjectReference objectReference)
		{
			if(TryWriteReference(objectReference))
				return;

			WriteLine("type");
			WriteLine(objectReference.Name);

			var type = objectReference.Value as Type;
			WriteLine(type.FullName);
		}

		void WriteType(Type type)
		{
			WriteLine(type.IsGenericType);

			if(type.IsGenericType)
			{
				WriteLine(type.GetGenericTypeDefinition().FullName);

				var genericArgs = type.GetGenericArguments();
				WriteLine(genericArgs.Length);
				foreach(var genericArg in genericArgs)
					WriteType(genericArg);
			}
			else
				WriteLine(type.FullName);
		}

		/// <summary>
		/// Checks if this object has already been serialized.
		/// </summary>
		/// <param name="objectReference"></param>
		/// <returns>true if object had already been serialized.</returns>
		bool TryWriteReference(ObjectReference objectReference)
		{
			foreach(var pair in ObjectReferences)
			{
				if(pair.Value.Value.GetHashCode() == objectReference.Value.GetHashCode())
				{
					WriteReference(objectReference, pair.Key);
					return true;
				}
			}

			ObjectReferences.Add(CurrentLine, objectReference);
			return false;
		}

		public object Deserialize(Stream stream)
		{
			if(stream == null || stream.Length == 0)
				throw new ArgumentNullException("stream");

			Reader = new StreamReader(stream);

			ObjectReferences.Clear();
			CurrentLine = 0;

			return StartRead().Value;
		}

		string ReadLine()
		{
			CurrentLine++;
			return Reader.ReadLine();
		}

		ObjectReference StartRead()
		{
			ObjectReference objReference = new ObjectReference();

			string type = ReadLine();
			int line = CurrentLine;

			switch(type)
			{
				case "null": ReadNull(objReference); break;
				case "reference": ReadReference(objReference); break;
				case "object": ReadObject(objReference); break;
				case "generic_enumerable": ReadGenericEnumerable(objReference); break;
				case "enumerable": ReadEnumerable(objReference); break;
				case "enum": ReadEnum(objReference); break;
				case "any": ReadAny(objReference); break;
				case "string": ReadString(objReference); break;
				case "memberinfo": ReadMemberInfo(objReference); break;
				case "type": ReadType(objReference); break;
				default: throw new SerializationException("Invalid object type {0} was serialized");
			}

			if(objReference.Value == null && type != "null")
				throw new SerializationException(string.Format("Failed to deserialize {0} {1} at line {2}!", type, objReference.Name, line));

			return objReference;
		}

		void ReadNull(ObjectReference objReference)
		{
			objReference.Name = ReadLine();
		}

		void ReadReference(ObjectReference objReference)
		{
			objReference.Name = ReadLine();
			int referenceLine = int.Parse(ReadLine());
			objReference.Value = ObjectReferences[referenceLine].Value;

			if(objReference.Value == null)
				throw new SerializationException(string.Format("Failed to obtain reference {0} at line {1}", objReference.Name, referenceLine));
		}

		void ReadObject(ObjectReference objReference)
		{
			AddReferenceToObject(objReference);

			objReference.Name = ReadLine();
			var type = ReadType();
			int numFields = int.Parse(ReadLine());

			objReference.Value = Activator.CreateInstance(type);
			for(int i = 0; i < numFields; ++i)
			{
				ObjectReference fieldReference = StartRead();

				var fieldInfo = type.GetField(fieldReference.Name, BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic);

				if(fieldInfo != null)
				{
					if(fieldInfo.FieldType == fieldReference.Value.GetType())
						fieldInfo.SetValue(objReference.Value, fieldReference.Value);
					else
						throw new SerializationException(string.Format("object type {0} can not be converted to type {1}", fieldReference.Value.GetType(), fieldInfo.FieldType));
				}
				else
					throw new MissingFieldException(string.Format("Failed to find field {0} in type {1}", fieldReference.Name, type != null ? type.Name : "[Unknown]"));
			}
		}

		void ReadEnumerable(ObjectReference objReference)
		{
			AddReferenceToObject(objReference);

			var numElements = int.Parse(ReadLine());
			objReference.Name = ReadLine();
			var type = ReadType();

			objReference.Value = Array.CreateInstance(type, numElements);
			var array = objReference.Value as Array;

			for(int i = 0; i != numElements; ++i)
				array.SetValue(StartRead().Value, i);
		}

		void ReadGenericEnumerable(ObjectReference objReference)
		{
			AddReferenceToObject(objReference);

			int elements = int.Parse(ReadLine());
			objReference.Name = ReadLine();

			var type = ReadType();

			objReference.Value = Activator.CreateInstance(type);

			if(type.Implements(typeof(IDictionary)))
			{
				var dict = objReference.Value as IDictionary;

				for(int i = 0; i < elements; i++)
				{
					var keyPair = StartRead().Value;
					var valueMethod = keyPair.GetType().GetProperty("Value");
					var keyMethod = keyPair.GetType().GetProperty("Key");

					dict.Add(keyMethod.GetValue(keyPair, null), valueMethod.GetValue(keyPair, null));
				}
			}
			else
			{
				var list = objReference.Value as IList;

				for(int i = 0; i < elements; i++)
					list[i] = StartRead().Value;
			}
		}

		void ReadAny(ObjectReference objReference)
		{
			objReference.Name = ReadLine();
			var type = ReadType();
			string valueString = ReadLine();

			objReference.Value = Converter.Convert(valueString, type);
		}

		void ReadString(ObjectReference objReference)
		{
			objReference.Name = ReadLine();
			objReference.Value = ReadLine();
		}

		void ReadEnum(ObjectReference objReference)
		{
			objReference.Name = ReadLine();
			var type = ReadType();
			string valueString = ReadLine();

			objReference.Value = Enum.Parse(type, valueString);
		}

		void ReadMemberInfo(ObjectReference objReference)
		{
			AddReferenceToObject(objReference);

			objReference.Name = ReadLine();
			var memberName = ReadLine();

			var declaringType = ReadType();
			var memberType = (MemberTypes)Enum.Parse(typeof(MemberTypes), ReadLine());

			MemberInfo memberInfo = null;

			switch(memberType)
			{
				case MemberTypes.Method:
					memberInfo = declaringType.GetMethod(memberName);
					break;
				case MemberTypes.Field:
					memberInfo = declaringType.GetField(memberName);
					break;
				case MemberTypes.Property:
					memberInfo = declaringType.GetProperty(memberName);
					break;
			}

			objReference.Value = memberInfo;
		}

		void ReadType(ObjectReference objReference)
		{
			AddReferenceToObject(objReference);

			objReference.Name = ReadLine();
			objReference.Value = ReadType();
		}

		Type ReadType()
		{
			bool isGeneric = bool.Parse(ReadLine());

			var type = GetType(ReadLine());

			if(isGeneric)
			{
				var numGenericArgs = int.Parse(ReadLine());
				var genericArgs = new Type[numGenericArgs];
				for(int i = 0; i < numGenericArgs; i++)
					genericArgs[i] = ReadType();

				return type.MakeGenericType(genericArgs);
			}

			return type;
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

			throw new TypeLoadException(string.Format("Could not localize type with name {0}", typeName));
		}

		static Type GetIEnumerableElementType(Type enumerableType)
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

		void AddReferenceToObject(ObjectReference objReference)
		{
			ObjectReferences.Add(CurrentLine - 1, objReference);
		}

		int CurrentLine { get; set; }

		public SerializationBinder Binder { get { return null; } set { } }
		public ISurrogateSelector SurrogateSelector { get { return null; } set { } }
		public StreamingContext Context { get { return new StreamingContext(StreamingContextStates.Persistence); } set { } }
	}
}