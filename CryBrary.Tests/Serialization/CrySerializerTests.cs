using System.Collections.Generic;
using System.IO;
using System.Linq;
using CryEngine.Serialization;
using NUnit.Framework;

namespace CryBrary.Tests.Serialization
{
	[TestFixture]
	public class CrySerializerTests : CryBraryTests
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

			public string NullString { get; set; }

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

				serializer = null;
				serializer = new CrySerializer();

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
				serializer.Serialize(stream, "Test str1ng_I5 V37y tEsTy%‹Œm´ð!");

				serializer = null;
				serializer = new CrySerializer();

				string testString = serializer.Deserialize(stream) as string;

				Assert.AreEqual("Test str1ng_I5 V37y tEsTy%‹Œm´ð!", testString);
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

				serializer = null;
				serializer = new CrySerializer();

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

				serializer = null;
				serializer = new CrySerializer();

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

				serializer = null;
				serializer = new CrySerializer();

				var array = serializer.Deserialize(stream) as object[];
				Assert.IsNotNull(array);
				Assert.IsNotEmpty(array);

				Assert.AreEqual("testString", array.ElementAt(0));
				Assert.AreEqual(1337, array.ElementAt(1));
				Assert.AreEqual(true, array.ElementAt(2));
			}
		}

		[Test]
		public void String_Array_With_MemoryStream()
		{
			using(var stream = new MemoryStream())
			{
				var list = new List<object>();

				list.Add("first_string");
				list.Add("second_string");
				list.Add("third_string");

				var serializer = new CrySerializer();
				serializer.Serialize(stream, list.ToArray());

				serializer = null;
				serializer = new CrySerializer();

				var array = serializer.Deserialize(stream) as object[];
				Assert.IsNotNull(array);
				Assert.IsNotEmpty(array);

				Assert.AreEqual("first_string", array.ElementAt(0));
				Assert.AreEqual("second_string", array.ElementAt(1));
				Assert.AreEqual("third_string", array.ElementAt(2));
			}
		}

		class Multiple_Reference_Test_Class
		{
			public Multiple_Reference_Test_Class()
			{
				ClassWithTestClassReference = new Class_Containing_Reference();
				TestClassReference = ClassWithTestClassReference.TestClass;

				TestClassSeperate = SetupTestClass();
			}

			public class Class_Containing_Reference
			{
				public Class_Containing_Reference()
				{
					TestClass = SetupTestClass();
				}

				public TestClass TestClass { get; set; }
			}

			public Class_Containing_Reference ClassWithTestClassReference { get; set; }
			public TestClass TestClassReference { get; set; }

			public TestClass TestClassSeperate { get; set; }
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

				serializer = null;
				serializer = new CrySerializer();

				referenceTestClass = serializer.Deserialize(stream) as Multiple_Reference_Test_Class;

				Assert.AreNotSame(referenceTestClass.ClassWithTestClassReference, referenceTestClass.TestClassSeperate);
				Assert.AreSame(referenceTestClass.ClassWithTestClassReference.TestClass, referenceTestClass.TestClassReference, "Objects were not the same; expected hash code: {0} but was: {1}",
					referenceTestClass.ClassWithTestClassReference.GetHashCode(), referenceTestClass.TestClassReference.GetHashCode());
			}
		}

		class Class_With_MemberInfo_Member
		{
			public Class_With_MemberInfo_Member()
			{
				MethodInfo = GetType().GetMethod("Method");
				FieldInfo = GetType().GetField("booleanField");
			}

			public void Method() { }

			public System.Reflection.MethodInfo MethodInfo { get; set; }
			public System.Reflection.FieldInfo FieldInfo { get; set; }

			public bool booleanField = true;
		}

		[Test]
		public void Class_With_MemberInfo_Members()
		{
			using(var stream = new MemoryStream())
			{
				var serializer = new CrySerializer();

				serializer.Serialize(stream, new Class_With_MemberInfo_Member());

				serializer = null;
				serializer = new CrySerializer();

				var memberInfoClass = serializer.Deserialize(stream) as Class_With_MemberInfo_Member;

				Assert.IsNotNull(memberInfoClass);

				Assert.AreSame(memberInfoClass.GetType().GetMethod("Method"), memberInfoClass.MethodInfo);
				Assert.AreSame(memberInfoClass.GetType().GetField("booleanField"), memberInfoClass.FieldInfo);

				Assert.IsTrue(memberInfoClass.booleanField);
			}
		}

		public interface Interface
		{
			void PureVirtualMethod();
		}
		
		public abstract class BaseClass : Interface
		{
			public virtual void NonoverriddenMethod()
			{
			}

			public virtual void OverriddenMethod()
			{
			}

			public void PureVirtualMethod()
			{
			}

			public bool BooleanProperty { get; set; }
			public int IntegerProperty { get; set; }
		}

		public class Class_Inherit_From_BaseClass : BaseClass
		{
			public override void OverriddenMethod()
			{
			}

			public string StringProperty { get; set; }
		}

		public class Class_Inherit_From_Class : Class_Inherit_From_BaseClass
		{
			public Class_Inherit_From_Class()
			{
				BooleanProperty = true;
				IntegerProperty = 13;
				StringProperty = "TestString";

				Vec3Property = new CryEngine.Vec3(1, 2, 3);
			}

			public CryEngine.Vec3 Vec3Property { get; set; }
		}

		[Test]
		public void Derivation()
		{
			using(var stream = new MemoryStream())
			{
				var serializer = new CrySerializer();

				serializer.Serialize(stream, new Class_Inherit_From_Class());

				serializer = null;
				serializer = new CrySerializer();

				var inheritClass = serializer.Deserialize(stream) as Class_Inherit_From_Class;

				Assert.IsNotNull(inheritClass);
				Assert.IsTrue(inheritClass.BooleanProperty);
				Assert.AreEqual(13, inheritClass.IntegerProperty);
				Assert.AreEqual("TestString", inheritClass.StringProperty);
				Assert.AreEqual(new CryEngine.Vec3(1, 2, 3), inheritClass.Vec3Property);
			}
		}

		[Test]
		public void GenericEnumerableception()
		{
			var dictionary = new Dictionary<int, List<TestClass>>();
			for(int i = 0; i < 10; i++)
			{
				var list = new List<TestClass>();
				list.Add(SetupTestClass());
				list.Add(null);

				dictionary.Add(i, list);
			}

			using(var stream = new MemoryStream())
			{
				var serializer = new CrySerializer();

				serializer.Serialize(stream, dictionary);
				dictionary = null;

				serializer = null;
				serializer = new CrySerializer();

				var deserializedDictionary = serializer.Deserialize(stream) as Dictionary<int, List<TestClass>>;

				Assert.IsNotNull(deserializedDictionary);
			}
		}
	}
}
