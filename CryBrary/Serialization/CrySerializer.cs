using System;
using System.IO;
using System.Reflection;
using System.Runtime.Serialization;

using System.Collections;
using System.Collections.Generic;
using System.Linq;

using CryEngine.Extensions;

namespace CryEngine.Serialization
{
	public class CrySerializer : IFormatter
	{
		StreamWriter Writer { get; set; }
		StreamReader Reader { get; set; }
		Assembly CallingAssembly { get; set; }
		FormatterConverter Converter { get; set; }

		Dictionary<string, ObjectReference> ObjectReferences;

		public CrySerializer()
		{
			Converter = new FormatterConverter();
			ObjectReferences = new Dictionary<string, ObjectReference>();
		}

		struct ObjectReference
		{
			public ObjectReference(string name, object value) 
				: this() 
			{ 
				Name = name;
				Value = value;
				FullName = Name;
			}

			public ObjectReference(string name, object value, ObjectReference owner)
				: this(name, value)
			{
				FullName = owner.FullName + "." + Name;
			}

			public ObjectReference(string name, object value, string fullName)
				: this(name, value)
			{
				FullName = fullName;
			}

			public string Name { get; set; }
			/// <summary>
			/// The full name including owner objects seperated by '.'
			/// </summary>
			public string FullName { get; set; }
			public object Value { get; set; }

			public override bool Equals(object obj)
			{
				if(obj is ObjectReference)
					return this.Equals(obj);

				return false;
			}

			public override int GetHashCode()
			{
				return base.GetHashCode();
			}

			public bool Equals(ObjectReference other)
			{
				return (this.Value.Equals(other.Value)) && (this.FullName.Equals(other.FullName));
			}

			/// <summary>
			/// Special behaviour; unlike Equals the operator methods do not compare Value!
			/// </summary>
			/// <param name="ref1"></param>
			/// <param name="ref2"></param>
			/// <returns></returns>
			public static bool operator ==(ObjectReference ref1, ObjectReference ref2)
			{
				return ref1.FullName.Equals(ref2.FullName);
			}

			public static bool operator !=(ObjectReference ref1, ObjectReference ref2)
			{
				return !(ref1 == ref2);
			}
		}

		public void Serialize(Stream stream, object graph)
		{
			try
			{
				Writer = new StreamWriter(stream);
				ObjectReferences.Clear();

				StartWrite(new ObjectReference("root", graph));
			}
			finally
			{
				if(Writer != null)
				{
					Writer.Dispose();
					Writer = null;
				}
			}
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
			{
				WriteNull(objectReference);
				return;
			}

			if(valueType.Implements(typeof(IEnumerable)))
				WriteEnumerable(objectReference);
			else if(!valueType.IsPrimitive && !valueType.IsEnum && valueType != typeof(string))
				WriteObject(objectReference);
			else
				WriteAny(objectReference);
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

			Writer.WriteLine("object");
			Writer.WriteLine(fields.Count);
			Writer.WriteLine(objectReference.Name);
			Writer.WriteLine(objectReference.FullName);
			Writer.WriteLine(objectReference.Value.GetType());

			foreach(var field in fields)
			{
				object fieldValue = field.GetValue(objectReference.Value);

				StartWrite(new ObjectReference(field.Name, fieldValue, objectReference));
			}
		}

		void WriteEnumerable(ObjectReference objectReference)
		{
			Writer.WriteLine("enumerable");

			var array = (objectReference.Value as IEnumerable).Cast<object>();
			Writer.WriteLine(array.Count());
			Writer.WriteLine(objectReference.Name);
			Writer.WriteLine(objectReference.FullName);
			Writer.WriteLine(array.GetType().GetElementType());

			for(int i = 0; i < array.Count(); i++)
				StartWrite(new ObjectReference(i.ToString(), array.ElementAt(i), objectReference));
		}

		void WriteNull(ObjectReference objectReference)
		{
			Writer.WriteLine("null");
			Writer.WriteLine(objectReference.Name);
		}

		void WriteAny(ObjectReference objectReference)
		{
			Writer.WriteLine("any");
			Writer.WriteLine(objectReference.Name);
			Writer.WriteLine(objectReference.FullName);
			Writer.WriteLine(objectReference.Value.GetType());
			Writer.WriteLine(Converter.ToString(objectReference.Value));
		}

		public object Deserialize(Stream stream)
		{
			try
			{
				Debug.LogAlways("Commencing deserialization");
				Reader = new StreamReader(stream);
				CallingAssembly = Assembly.GetCallingAssembly();
				ObjectReferences.Clear();

				Debug.LogAlways("Deserializing");

				return StartRead().Value;
			}
			finally
			{
				if(Reader != null)
				{
					Reader.Dispose();
					Reader = null;
				}
			}
		}

		ObjectReference StartRead()
		{
			Debug.LogAlways("Start read");
			ObjectReference result = default(ObjectReference);

			switch(Reader.ReadLine())
			{
				case "null": result = ReadNull(); break;
				case "object": result = ReadObject(); break;
				case "enumerable": result = ReadEnumerable(); break;
				case "any": result = ReadAny(); break;
				default: break;
			}

			if(result.Equals(default(ObjectReference)))
				throw new Exception(string.Format("Could not deserialize object!"));
			
			Debug.LogAlways("End start read");
			return result;
		}

		ObjectReference ReadNull()
		{
			Debug.LogAlways("Reading null");
			return new ObjectReference(Reader.ReadLine(), null);
		}

		ObjectReference ReadObject()
		{
			Debug.LogAlways("Reading object");
			int numFields = int.Parse(Reader.ReadLine());
			string name = Reader.ReadLine();
			string fullName = Reader.ReadLine();
			string typeName = Reader.ReadLine();

			if(ObjectReferences.ContainsKey(fullName))
			{
				// We have to read anyway, in order to get to the correct line for the next object.
				for(int i = 0; i != numFields; ++i)
					StartRead();

				return ObjectReferences[fullName];
			}

			object objectInstance = CreateObjectInstance(typeName);

			for(int i = 0; i != numFields; ++i)
			{
				ObjectReference fieldReference = StartRead();
				var fieldInfo = objectInstance.GetType().GetField(fieldReference.Name);

				fieldInfo.SetValue(objectInstance, fieldReference.Value);
			}

			ObjectReferences.Add(fullName, new ObjectReference(name, objectInstance, fullName));
			Debug.LogAlways("Done reading object");
			return ObjectReferences.Last().Value;
		}

		ObjectReference ReadEnumerable()
		{
			Debug.LogAlways("Reading enumerable");
			int elements = int.Parse(Reader.ReadLine());
			string name = Reader.ReadLine();
			string fullName = Reader.ReadLine();
			string typeName = Reader.ReadLine();

			if(ObjectReferences.ContainsKey(fullName))
			{
				// We have to read anyway, in order to get to the correct line for the next object.
				for(int i = 0; i != elements; ++i)
					StartRead();

				return ObjectReferences[fullName];
			}

			object objectInstance = CreateObjectInstance(typeName);
			var array = (objectInstance as IEnumerable).Cast<object>().ToArray();

			for(int i = 0; i != elements; ++i)
				array.SetValue(StartRead().Value, i);

			ObjectReferences.Add(fullName, new ObjectReference(name, array, fullName));
			Debug.LogAlways("Done reading enumerable");
			return ObjectReferences.Last().Value;
		}

		ObjectReference ReadAny()
		{
			Debug.LogAlways("Reading any");
			string name = Reader.ReadLine();
			string fullName = Reader.ReadLine();

			if(ObjectReferences.ContainsKey(fullName))
			{
				// In order to get to the next object's line, we have to read anyway.
				Reader.ReadLine(); // type
				Reader.ReadLine(); // value

				return ObjectReferences[fullName];
			}

			Type type = Type.GetType(Reader.ReadLine());
			object value = Converter.Convert(Reader.ReadLine(), type);

			ObjectReferences.Add(fullName, new ObjectReference(name, value, fullName));
			Debug.LogAlways("Done reading any");
			return ObjectReferences.Last().Value;
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

		object CreateObjectInstance(string typeName)
		{
			Type type = null;

			var script = ScriptCompiler.CompiledScripts.FirstOrDefault(x => x.ScriptType.FullName.Equals(typeName));
			if(script != default(CryScript))
				type = script.ScriptType;

			type = type ?? System.Type.GetType(typeName);

			if(type == null)
				throw new Exception(string.Format("Could not localize type with name {0}", typeName));

			if(type.GetConstructor(System.Type.EmptyTypes) != null || type.IsValueType)
				return System.Activator.CreateInstance(type);

			throw new Exception(string.Format("Could not serialize type {0} since it did not containg a parameterless constructor", type.Name));
		}

		public SerializationBinder Binder { get { return null; } set { } }
		public ISurrogateSelector SurrogateSelector { get { return null; } set { } }
		public StreamingContext Context { get { return new StreamingContext(StreamingContextStates.Persistence); } set { } }
	}
}