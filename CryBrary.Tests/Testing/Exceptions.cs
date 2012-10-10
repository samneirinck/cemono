using System;
using CryEngine.Testing;

namespace CryBrary.Tests.UnitTester
{
	public class Exceptions
	{
		[Xunit.Fact]
		public void Throw_Valid()
		{
			var obj = new object();

			Assert.Throws<InvalidCastException>(() =>
			{
				var myInt = (int)obj;
			});
		}

		[Xunit.Fact]
		public void Throw_Invalid()
		{
			object obj = 1;

            Xunit.Assert.Throws<AssertionFailedException>(() =>
            {
                Assert.Throws<InvalidCastException>(() =>
                {
                    var myInt = (int)obj;
                });
            });
		}
	}
}
