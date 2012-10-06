using System;
using NUnit.Framework;

using CryEngine.Utils;

namespace CryBrary.Tests.Misc
{
	[TestFixture]
	public class UnusedMarkerTests
	{
		[Test]
		public void Float()
		{
			float myFloat = UnusedMarker.Float;
			Assert.IsTrue(UnusedMarker.IsUnused(myFloat));

			myFloat = 998.4f;
			Assert.IsFalse(UnusedMarker.IsUnused(myFloat));
		}

		[Test]
		public void Integer()
		{
			int myInt = UnusedMarker.Integer;
			Assert.IsTrue(UnusedMarker.IsUnused(myInt));

			myInt = 5;
			Assert.IsFalse(UnusedMarker.IsUnused(myInt));
		}

		[Test]
		public void UnsignedInteger()
		{
			uint myUint = UnusedMarker.UnsignedInteger;
			Assert.IsTrue(UnusedMarker.IsUnused(myUint));

			myUint = 51;
			Assert.IsFalse(UnusedMarker.IsUnused(myUint));
		}
	}
}
