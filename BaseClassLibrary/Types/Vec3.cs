using System.Runtime.InteropServices;

namespace CryEngine
{
	/// <summary>
	/// The Vec3 struct is used for all 3D coordinates within the engine.
	/// TODO: Implement a Quat class for rotation in place of angles.
	/// </summary>
    [System.Serializable]
    [StructLayout(LayoutKind.Sequential)]
    public struct Vec3
    {
        #region Properties
        public float X { get; set; }
        public float Y { get; set; }
        public float Z { get; set; }
        #endregion

        #region Constructor(s)
        public Vec3(float x, float y, float z)
            : this()
        {
            X = x;
            Y = y;
            Z = z;
        }

        public Vec3(float x, float y)
            : this()
        {
            X = x;
            Y = y;
            Z = 0f;
        }
        #endregion

        #region Methods
        public void Normalize()
        {
			this = this.Normalized;
        }

        public void Zero() { X = 0f; Y = 0f; Z = 0f; }

		public Vec3 Normalized
		{
			get
			{
				if(Length > 0)
					return new Vec3(X / Length, Y / Length, Z / Length);

				return new Vec3();
			}
		}

        public float Length
        {
            get
            {
                return (float)Math.Sqrt((X * X) + (Y * Y) + (Z * Z));
            }
        }

        public float SqrLength
        {
            get
            {
                return (X * X) + (Y * Y) + (Z * Z);
            }
        }

        #endregion

        #region Operators

        /// <summary>
        /// Negate this vector.
        /// </summary>
        public static Vec3 operator -(Vec3 v)
        {
            return new Vec3(-v.X, -v.Y, -v.Z);
        }

        /// <summary>
        /// Add two vectors component-wise.
        /// </summary>
        public static Vec3 operator +(Vec3 v1, Vec3 v2)
        {
            return new Vec3(v1.X + v2.X, v1.Y + v2.Y, v1.Z + v2.Z);
        }

        /// <summary>
        /// Subtract two vectors component-wise.
        /// </summary>
        public static Vec3 operator -(Vec3 v1, Vec3 v2)
        {
            return new Vec3(v1.X - v2.X, v1.Y - v2.Y, v1.Z - v2.Z);
        }

        /// <summary>
        /// Multiply this vector by a scalar.
        /// </summary>
        public static Vec3 operator *(Vec3 v, float s)
        {
            return new Vec3(v.X * s, v.Y * s, v.Z * s);
        }

        /// <summary>
        /// Multiply this vector by a scalar.
        /// </summary>
        public static Vec3 operator *(float s, Vec3 v)
        {
            return new Vec3(v.X * s, v.Y * s, v.Z * s);
        }

        public static bool operator ==(Vec3 v1, Vec3 v2)
        {
            return (v1.X == v2.X && v1.Y == v2.Y && v1.Z == v2.Z);
        }

        public static bool operator !=(Vec3 v1, Vec3 v2)
        {
            return (v1.X != v2.X || v1.Y != v2.Y || v1.Z == v2.Z);
        }

        #endregion

        #region Statics
        /// <summary>
        /// Perform the dot product on two vectors.
        /// </summary>
        public static float Dot(Vec3 a, Vec3 b)
        {
            return a.X * b.X + a.Y * b.Y + a.Z * b.Z;
        }

        /// <summary>
        /// Perform the cross product on two vectors.
        /// </summary>
        public static Vec3 Cross(Vec3 a, Vec3 b)
        {
            return new Vec3(a.Y * b.Z - a.Z * b.Y, a.Z * b.X - a.X * b.Z, a.X * b.Y - a.Y * b.X);
        }

		/// <summary>
		/// Linearly interpolates from vector a to b using the value of control.
		/// </summary>
		/// <param name="a">The "from" vector.</param>
		/// <param name="b">The "to" vector.</param>
		/// <param name="control">The position between a and b that the function should return.</param>
		/// <returns></returns>
		public static Vec3 Lerp(Vec3 a, Vec3 b, float control)
		{
			control = Math.Clamp(control, 0, 1);
			var leftControl = 1 - control;
			return new Vec3(a.X * leftControl + b.X * control, a.Y * leftControl + b.Y * control, a.Z * leftControl + b.Z * control);
		}
        #endregion

        #region Overrides
        public override string ToString()
        {
            return string.Format("{0},{1},{2}", X.ToString(), Y.ToString(), Z.ToString());
        }

        public override bool Equals(object obj)
        {
            if (obj is Vec3)
                return (Vec3)obj == this;

            return false;
        }

        public override int GetHashCode()
        {
            return base.GetHashCode();
        }
        #endregion
    }
}
