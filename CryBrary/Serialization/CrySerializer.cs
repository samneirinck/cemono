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

		public CrySerializer()
		{
			Converter = new FormatterConverter();
		}

		struct ObjectReference
		{
			public ObjectReference(string name, object value) : this() { Name = name; Value = value; }

			public string Name { get; set; }
			public object Value { get; set; }
		}

		public void Serialize(Stream stream, object graph)
		{
			try
			{
				Writer = new StreamWriter(stream);
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
				WriteNull(objectReference);

			Type valueType = objectReference.Value.GetType();
			if(!IsTypeAllowed(valueType))
				return;

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
			Writer.WriteLine(objectReference.Value.GetType());

			foreach(var field in fields)
			{
				object fieldValue = field.GetValue(objectReference.Value);

				StartWrite(new ObjectReference(field.Name, fieldValue));
			}
		}

		void WriteEnumerable(ObjectReference objectReference)
		{
			Writer.WriteLine("enumerable");

			var array = (objectReference.Value as IEnumerable).Cast<object>();
			Writer.WriteLine(array.Count());
			Writer.WriteLine(objectReference.Name);
			Writer.WriteLine(array.ElementAt(0).GetType());

			for(int i = 0; i < array.Count(); i++)
				StartWrite(new ObjectReference(i.ToString(), array.ElementAt(i)));
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
			Writer.WriteLine(objectReference.GetType());
			Writer.WriteLine(Converter.ToString(objectReference.Value));
		}

		public object Deserialize(Stream stream)
		{
			try
			{
				Reader = new StreamReader(stream);
				CallingAssembly = Assembly.GetCallingAssembly();

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
			switch(Reader.ReadLine())
			{
				case "null": return ReadNull();
				case "object": return ReadObject();
				case "enumerable": return ReadEnumerable();
				case "any": return ReadAny();
				default: break;
			}

			throw new Exception(string.Format("Cannot deserialize object!"));
		}

		ObjectReference ReadNull()
		{
			return new ObjectReference(Reader.ReadLine(), null);
		}

		ObjectReference ReadObject()
		{
			int numFields = int.Parse(Reader.ReadLine());
			string name = Reader.ReadLine();
			string typeName = Reader.ReadLine();

			object objectInstance = CreateObjectInstance(typeName);

			for(int i = 0; i != numFields; ++i)
			{
				ObjectReference fieldReference = StartRead();
				var fieldInfo = objectInstance.GetType().GetField(fieldReference.Name);

				fieldInfo.SetValue(objectInstance, fieldReference.Value);
			}

			return new ObjectReference(name, objectInstance);
		}

		ObjectReference ReadEnumerable()
		{
			int elements = int.Parse(Reader.ReadLine());
			string name = Reader.ReadLine();
			string typeName = Reader.ReadLine();

			object objectInstance = CreateObjectInstance(typeName);
			var array = (objectInstance as IEnumerable).Cast<object>().ToArray();

			for(int i = 0; i != elements; ++i)
				array.SetValue(StartRead().Value, i);

			return new ObjectReference(name, array);
		}

		ObjectReference ReadAny()
		{
			string name = Reader.ReadLine();
			Type type = Type.GetType(Reader.ReadLine());
			object value = Converter.Convert(Reader.ReadLine(), type);

			return new ObjectReference(name, value);
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