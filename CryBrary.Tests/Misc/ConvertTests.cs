using System;
using NUnit.Framework;

namespace CryBrary.Tests.Misc
{
	[TestFixture]
	public class ConvertTests
	{
		[Test]
		public void FromString_ValidBool_BoolResult()
		{
			// Arrange
			const string allLowercaseBool = "true";
			const string capitalizedBool = "True";
			const string allUppercaseBool = "TRUE";

			// Act
			object allLowercaseBoolResult = CryEngine.Convert.FromString(CryEngine.EntityPropertyType.Bool, allLowercaseBool);
			object capitalizedBoolResult = CryEngine.Convert.FromString(CryEngine.EntityPropertyType.Bool, capitalizedBool);
			object allUppercaseBoolResult = CryEngine.Convert.FromString(CryEngine.EntityPropertyType.Bool, allUppercaseBool);

			// Assert
			Assert.IsTrue(allLowercaseBoolResult is bool);
			Assert.IsTrue((bool)allLowercaseBoolResult == true);

			Assert.IsTrue(capitalizedBoolResult is bool);
			Assert.IsTrue((bool)capitalizedBoolResult == true);

			Assert.IsTrue(allUppercaseBoolResult is bool);
			Assert.IsTrue((bool)allUppercaseBoolResult == true);
		}

		[Test]
		[ExpectedException(typeof(ArgumentNullException))]
		public void FromString_NullBool_ArgumentNullException()
		{
			// Arrange
			string input = null;

			// Act
			CryEngine.Convert.FromString(CryEngine.EntityPropertyType.Bool, input);

			// Assert
			// Throws exception
		}
	}
}
