using System.Runtime.InteropServices;

namespace CryEngine
{
	/// <summary>
	/// The Vec3 struct is used for all 3D coordinates within the engine.
	/// </summary>
    public struct Vec3
    {
        #region Properties
        public float X { get; set; }
        public float Y { get; set; }
        public float Z { get; set; }
        #endregion

        #region Constructor(s)
		public Vec3(Quat q)
			: this()
		{
			Y = (float)Math.Asin(Math.Max(-1.0f, Math.Min(1.0f, -(q.Axis.X * q.Axis.Z - q.Angle * q.Axis.Y) * 2.0f)));
			if(Math.Abs(Math.Abs(Y) - (Math.PI * 0.5)) < 0.01)
			{
				X = 0;
				Z = (float)Math.Atan2(-2 * (q.Axis.X * q.Axis.Y - q.Angle * q.Axis.Z), 1 - (q.Axis.X * q.Axis.X + q.Axis.Z * q.Axis.Z) * 2);
			}
			else
			{
				X = (float)Math.Atan2((q.Axis.Y * q.Axis.Z + q.Angle * q.Axis.X) * 2, 1 - (q.Axis.X * q.Axis.X + q.Axis.Y * q.Axis.Y) * 2);
				Z = (float)Math.Atan2((q.Axis.X * q.Axis.Y + q.Angle * q.Axis.Z) * 2, 1 - (q.Axis.Z * q.Axis.Z + q.Axis.Y * q.Axis.Y) * 2);
			}
		}

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
			if(Length > 0)
			{
				X /= Length;
				Y /= Length;
				Z /= Length;
			}
			else
			{
				X = 0;
				Y = 0;
				Z = 0;
			}
        }

        public void Zero() { X = 0f; Y = 0f; Z = 0f; }

		public Vec3 Normalized
		{
			get
			{
				Vec3 vec = new Vec3(X, Y, Z);
				vec.Normalize();

				return vec;
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

		public bool IsEquivalent(Vec3 v1, float epsilon = 0.05f)
		{
			return ((Math.Abs(X - v1.X) <= epsilon) && (Math.Abs(Y - v1.Y) <= epsilon) && (Math.Abs(Z - v1.Z) <= epsilon));	
		}

		public float Dot(Vec3 vec)
		{
			return Dot(this, vec);
		}

		public Vec3 Cross(Vec3 vec)
		{
			return Cross(this, vec);
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

		public static float operator *(Vec3 v0, Vec3 v1)
		{
			return v0.Dot(v1);
		}

		public static float operator |(Vec3 v0, Vec3 v1)
		{
			return v0.Dot(v1);
		}

		public static Vec3 operator %(Vec3 v0, Vec3 v1)
		{
			return v0.Cross(v1);
		}

        public static bool operator ==(Vec3 v1, Vec3 v2)
        {
            return (v1.X == v2.X && v1.Y == v2.Y && v1.Z == v2.Z);
        }

        public static bool operator !=(Vec3 v1, Vec3 v2)
        {
            return (v1.X != v2.X || v1.Y != v2.Y || v1.Z == v2.Z);
        }

		public static Vec3 operator /(Vec3 v, float k)
		{
			return new Vec3(v.X / k, v.Y / k, v.Z / k);
		}

		public static Vec3 operator /(Vec3 v, double k)
		{
			return new Vec3(v.X / (float)k, v.Y / (float)k, v.Z / (float)k);
		}

		public static bool operator >(Vec3 v1, Vec3 v2)
		{
			return v1.Length > v2.Length;
		}

		public static bool operator <(Vec3 v1, Vec3 v2)
		{
			return v1.Length < v2.Length;
		}

		public static bool operator >=(Vec3 v1, Vec3 v2)
		{
			return v1.Length >= v2.Length;
		}

		public static bool operator <=(Vec3 v1, Vec3 v2)
		{
			return v1.Length <= v2.Length;
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
