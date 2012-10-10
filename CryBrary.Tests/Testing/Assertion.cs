using CryEngine.Testing;

namespace CryBrary.Tests.UnitTester
{
	public class Assertion
	{
		[Xunit.Fact]
		public void ReferenceEquals_Valid()
		{
			var lhs = new object();
			var rhs = lhs;

			Assert.IsTrue(lhs == rhs);
		}

		[Xunit.Fact]
		public void ReferenceEquals_Invalid()
		{
			var lhs = new object();
			var rhs = new object();

            Xunit.Assert.Throws<AssertionFailedException>(() =>
            {
                Assert.IsTrue(lhs == rhs);
            });
		}
	}
}
