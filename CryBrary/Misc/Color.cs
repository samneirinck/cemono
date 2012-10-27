using System;

namespace CryEngine
{
	/// <summary>
	/// Defines a color in terms of its red, green, blue and alpha values.
	/// </summary>
	public struct Color
	{
		/// <summary>
		/// Constructs a new color object specifying the red, green, blue and alpha values.
		/// </summary>
		/// <param name="red"></param>
		/// <param name="green"></param>
		/// <param name="blue"></param>
		/// <param name="alpha"></param>
		public Color(float red, float green, float blue, float alpha)
			: this()
		{
#if !(RELEASE && RELEASE_DISABLE_CHECKS)
			if(!MathHelpers.IsInRange(red, 0, 1) || !MathHelpers.IsInRange(green, 0, 1) || !MathHelpers.IsInRange(blue, 0, 1) || !MathHelpers.IsInRange(alpha, 0, 1))
				throw new ArgumentException("Color and alpha values must be between 0 and 1.");
#endif

			R = red;
			G = green;
			B = blue;
			A = alpha;
		}

		/// <summary>
		/// Constructs a new color object specifying the red, green, and blue values.
		/// </summary>
		/// <param name="red"></param>
		/// <param name="green"></param>
		/// <param name="blue"></param>
		public Color(float red, float green, float blue) : this(red, green, blue, 1) { }

		/// <summary>
		/// Constructs a new greyscale color object.
		/// </summary>
		/// <param name="brightness">The brightness of the object, where 0 is black, and 1 is white.</param>
		public Color(float brightness) : this(brightness, brightness, brightness) { }

		#region Overrides
		public override bool Equals(object obj)
		{
			if(obj is Color)
				return (Color)obj == this;

			return false;
		}

		public override string ToString()
		{
			return String.Format("R {0} G {1} B {2} A {3}", R, G, B, A);
		}

		public override int GetHashCode()
		{
			return base.GetHashCode();
		}
		#endregion

		#region Operators
		public static bool operator ==(Color col1, Color col2)
		{
			return col1.R == col2.R && col1.G == col2.G && col1.B == col2.B && col1.A == col2.A;
		}

		public static bool operator !=(Color col1, Color col2)
		{
			return col1.R != col2.R || col1.G != col2.G || col1.B != col2.B || col1.A != col2.A;
		}
		#endregion

		/// <summary>
		/// The red value of the color.
		/// </summary>
		public float R { get; set; }

		/// <summary>
		/// The green value of the color.
		/// </summary>
		public float G { get; set; }

		/// <summary>
		/// The blue value of the color.
		/// </summary>
		public float B { get; set; }

		/// <summary>
		/// The alpha value of the color.
		/// </summary>
		public float A { get; set; }

		#region Statics
		public static Color Red { get { return new Color(1, 0, 0); } }
		public static Color Blue { get { return new Color(0, 0, 1); } }
		public static Color Green { get { return new Color(0, 1, 0); } }

		public static Color Black { get { return new Color(0); } }
		public static Color White { get { return new Color(1); } }
		#endregion

		public static implicit operator Vec3(Color clr)
		{
			return new Vec3(clr.R, clr.G, clr.B);
		}
	}
}
