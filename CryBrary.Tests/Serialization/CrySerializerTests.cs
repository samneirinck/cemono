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

				var deserializedObject = serializer.Deserialize(stream);
				Assert.IsNotNull(deserializedObject);

				var deserializedDictionary = deserializedObject as Dictionary<string, int>;

				Assert.IsNotEmpty(deserializedDictionary);

				Assert.AreEqual(2, deserializedDictionary.Count);

				var firstKey = deserializedDictionary.First().Key;
				Assert.AreEqual(firstKey, "test1");
				Assert.AreEqual(deserializedDictionary[firstKey], 1);

				var secondKey = deserializedDictionary.ElementAt(1).Key;
				Assert.AreEqual(secondKey, "test2");
				Assert.AreEqual(deserializedDictionary[secondKey], 1); // fail on purpose for unit testing, fix after :p
			}
		}
	}
}
