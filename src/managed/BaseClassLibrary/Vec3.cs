using System.Runtime.InteropServices;
using System;

namespace CryEngine
{
    [Serializable]
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
            float length = Length;

            if (length > 0)
            {
                X = X / length;
                Y = Y / length;
                Z = Z / length;
            }
            else
            {
                this = Vec3.Zero;
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

        #region Statics
        private static Vec3 _zero = new Vec3(0f, 0f, 0f);
        public static Vec3 Zero
        {
            get
            {
                return _zero;
            }
        }
        #endregion
    }
}
