using System.IO;
using System.Collections.Generic;

using System.Linq;

using CryEngine.Serialization;

using NUnit.Framework;

namespace CryBrary.Tests.Serialization
{
	[TestFixture]
	public class CrySerializerTests
	{
		public class TestClass
		{
			public class NestedClass
			{
				public NestedClass() { }

				public NestedClass(NestedEnum enumVal)
				{
					NestedEnum = enumVal;
				}
				 
				public NestedEnum NestedEnum { get; set; }
			}

			public enum NestedEnum
			{
				Nested_Quite,
				Nested_NotQuite
			}

			public string String { get; set; }
			public int Integer { get; set; }
			public bool Boolean { get; set; }

			public NestedClass nestedClass { get; set; }
		}

		[Test]
		public void SerializeDeserializeTestClassWithMemoryStream()
		{
			using(var stream = new MemoryStream())
			{
				var testClass = new TestClass();
				testClass.Integer = 3;
				testClass.String = "testString";
				testClass.Boolean = true;

				testClass.nestedClass = new TestClass.NestedClass(TestClass.NestedEnum.Nested_NotQuite);

				var serializer = new CrySerializer();
				serializer.Serialize(stream, testClass);

				testClass = null;

				testClass = serializer.Deserialize(stream) as TestClass;
				Assert.IsNotNull(testClass);

				Assert.IsTrue(testClass.Boolean);
				Assert.AreEqual(3, testClass.Integer);
				Assert.AreEqual("testString", testClass.String);

				Assert.IsNotNull(testClass.nestedClass);

				Assert.AreEqual(testClass.nestedClass.NestedEnum, TestClass.NestedEnum.Nested_NotQuite);
			}
		}

		[Test]
		public void SerializeDeserializeStringWithMemoryStream()
		{
			using(var stream = new MemoryStream())
			{
				var serializer = new CrySerializer();
				serializer.Serialize(stream, "test string is very testy");

				string testString = serializer.Deserialize(stream) as string;

				Assert.AreEqual("test string is very testy", testString);
			}
		}

		[Test]
		public void SerializeDeserializeListWithMemoryStream()
		{
			using(var stream = new MemoryStream())
			{
				var list = new List<string>();
				list.Add("test1");
				list.Add("test2");

				var serializer = new CrySerializer();
				serializer.Serialize(stream, list);

				list = null;
				var deserialized = serializer.Deserialize(stream) as List<string>;

				Assert.IsNotNull(deserialized);
				Assert.AreEqual(2, deserialized.Count());

				Assert.AreEqual("test1", deserialized.ElementAt(0));
				Assert.AreEqual("test2", deserialized.ElementAt(1));
			}
		}

		[Test]
		public void SerializeDeserializeDictionaryWithMemoryStream()
		{
			using(var stream = new MemoryStream())
			{
				var dictionary = new Dictionary<string, int>();
				dictionary.Add("test1", 1);
				dictionary.Add("test2", 2);

				var serializer = new CrySerializer();
				serializer.Serialize(stream, dictionary);

				var deserializedDictionary = serializer.Deserialize(stream) as Dictionary<string, int>;
				Assert.IsNotNull(deserializedDictionary);

				Assert.AreEqual(2, deserializedDictionary.Count);

				Assert.AreEqual(2, deserializedDictionary.Count);

				var firstKey = deserializedDictionary.First().Key;
				Assert.AreEqual("test1", firstKey);
				Assert.AreEqual(1, deserializedDictionary[firstKey]);

				var secondKey = deserializedDictionary.ElementAt(1).Key;
				Assert.AreEqual("test2", secondKey);
				Assert.AreEqual(2, deserializedDictionary[secondKey]);
			}
		}

		[Test]
		public void SerializeDeserializeObjectArrayWithMemoryStream()
		{
			using(var stream = new MemoryStream())
			{
				var list = new List<object>();

				list.Add("testString");
				list.Add(1337);
				list.Add(true);

				var serializer = new CrySerializer();
				serializer.Serialize(stream, list.ToArray());

				var array = serializer.Deserialize(stream) as object[];
				Assert.IsNotNull(array);
				Assert.IsNotEmpty(array);

				Assert.AreEqual("testString", array.ElementAt(0));
				Assert.AreEqual(1337, array.ElementAt(1));
				Assert.AreEqual(true, array.ElementAt(2));
			}
		}
	}
}
