using System;
using CryEngine.Testing;

using TestAttribute = NUnit.Framework.TestAttribute;
using ExpectedException = NUnit.Framework.ExpectedExceptionAttribute;
using TestFixture = NUnit.Framework.TestFixtureAttribute;

namespace CryBrary.Tests.UnitTester
{
	[TestFixture]
	public class Exceptions
	{
		[TestAttribute]
		public void Throw_Valid()
		{
			var obj = new object();

			Assert.Throws<InvalidCastException>(() =>
			{
				var myInt = (int)obj;
			});
		}

		[TestAttribute]
		[ExpectedException(typeof(AssertionFailedException))]
		public void Throw_Invalid()
		{
			object obj = 1;

			Assert.Throws<InvalidCastException>(() =>
			{
				var myInt = (int)obj;
			});
		}
	}
}
