using Cemono;
using NUnit.Framework;

namespace BaseClassLibrary.Tests
{
    [TestFixture]
    public class Vec3Tests
    {
        [Test]
        public void Equals_EqualVec3Objects_True()
        {
            Vec3 v1 = new Vec3(1, 2, 3);
            Vec3 v2 = new Vec3(1, 2, 3);

            Assert.IsTrue(v1.Equals(v2));
        }

        [Test]
        public void Length_NormalizedVec3_True()
        {
            Vec3 v = new Vec3(1, 0, 0);

            Assert.IsTrue(v.Length == 1);
        }

        [Test]
        public void Normalize_Vec3_LengthIsEqualToOne()
        {
            Vec3 v = new Vec3(26, 23, 135);
            v.Normalize();
            Assert.IsTrue(v.Length == 1);
        }
    }
}
