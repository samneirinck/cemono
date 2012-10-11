using System;

using CryEngine;
using CryEngine.Utils;
using Xunit;

namespace CryBrary.Tests.Misc
{
	public class UnusedMarkerTests
	{
		[Fact]
		public void Float()
		{
			float myFloat = UnusedMarker.Float;
            Assert.True(UnusedMarker.IsUnused(myFloat));

			myFloat = 998.4f;
			Assert.False(UnusedMarker.IsUnused(myFloat));
		}

		[Fact]
		public void Integer()
		{
			int myInt = UnusedMarker.Integer;
			Assert.True(UnusedMarker.IsUnused(myInt));

			myInt = 5;
			Assert.False(UnusedMarker.IsUnused(myInt));
		}

		[Fact]
		public void UnsignedInteger()
		{
			uint myUint = UnusedMarker.UnsignedInteger;
			Assert.True(UnusedMarker.IsUnused(myUint));

			myUint = 51;
			Assert.False(UnusedMarker.IsUnused(myUint));
		}

		[Fact]
		public void Vec3()
		{
			Vec3 vec3 = UnusedMarker.Vec3;
			Assert.True(UnusedMarker.IsUnused(vec3));

			vec3 = new Vec3(0, 250, 3);
			Assert.False(UnusedMarker.IsUnused(vec3));
		}
	}
}
