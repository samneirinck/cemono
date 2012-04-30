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

		static TestClass SetupTestClass()
		{
			var testClass = new TestClass();
			testClass.Integer = 3;
			testClass.String = "testString";
			testClass.Boolean = true;

			testClass.nestedClass = new TestClass.NestedClass(TestClass.NestedEnum.Nested_NotQuite);

			return testClass;
		}

		[Test]
		public void TestClass_With_MemoryStream()
		{
			using(var stream = new MemoryStream())
			{
				var serializer = new CrySerializer();
				serializer.Serialize(stream, SetupTestClass());

				var testClass = serializer.Deserialize(stream) as TestClass;
				Assert.IsNotNull(testClass);

				Assert.IsTrue(testClass.Boolean);
				Assert.AreEqual(3, testClass.Integer);
				Assert.AreEqual("testString", testClass.String);

				Assert.IsNotNull(testClass.nestedClass);

				Assert.AreEqual(testClass.nestedClass.NestedEnum, TestClass.NestedEnum.Nested_NotQuite);
			}
		}

		[Test]
		public void String_With_MemoryStream()
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
		public void List_With_MemoryStream()
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
		public void Dictionary_With_MemoryStream()
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
		public void Object_Array_With_MemoryStream()
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

		class Multiple_Reference_Test_Class
		{
			public Multiple_Reference_Test_Class()
			{
				classWithTestClassReference = new Class_Containing_Reference();
				testClassReference = classWithTestClassReference.TestClass;

				testClassSeperate = SetupTestClass();
			}

			public class Class_Containing_Reference
			{
				public Class_Containing_Reference()
				{
					TestClass = SetupTestClass();
				}

				public TestClass TestClass { get; set; }
			}

			public Class_Containing_Reference classWithTestClassReference;
			public TestClass testClassReference { get; set; }

			public TestClass testClassSeperate { get; set; }
		}

		[Test]
		public void Reference_Object_Serialization()
		{
			using(var stream = new MemoryStream())
			{
				var referenceTestClass = new Multiple_Reference_Test_Class();

				var serializer = new CrySerializer();
				serializer.Serialize(stream, referenceTestClass);

				referenceTestClass = null;

				referenceTestClass = serializer.Deserialize(stream) as Multiple_Reference_Test_Class;

				Assert.AreNotSame(referenceTestClass.classWithTestClassReference, referenceTestClass.testClassSeperate);
				Assert.AreSame(referenceTestClass.classWithTestClassReference.TestClass, referenceTestClass.testClassReference, "ObjectReference validation failed; deserialized objects were not the same!");
			}
		}
	}
}
