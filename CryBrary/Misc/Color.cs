using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CryEngine
{
	/// <summary>
	/// Defines a colour in terms of its red, green, blue and alpha values.
	/// </summary>
	public struct Color
	{
		/// <summary>
		/// Constructs a new colour object specifying the red, green, blue and alpha values.
		/// </summary>
		/// <param name="r"></param>
		/// <param name="g"></param>
		/// <param name="b"></param>
		/// <param name="alpha"></param>
		public Color(float red, float green, float blue, float alpha)
			: this()
		{
			if(!Math.IsInRange(red, 0, 1) || !Math.IsInRange(green, 0, 1) || !Math.IsInRange(blue, 0, 1) || !Math.IsInRange(alpha, 0, 1))
			{
				throw new ArgumentException("Color and alpha values must be between 0 and 1.");
			}

			this.R = red;
			this.G = green;
			this.B = blue;
			this.A = alpha;
		}

		/// <summary>
		/// Constructs a new colour object specifying the red, green, and blue values.
		/// </summary>
		/// <param name="r"></param>
		/// <param name="g"></param>
		/// <param name="b"></param>
		public Color(float red, float green, float blue) : this(red, green, blue, 1) { }

		/// <summary>
		/// Constructs a new greyscale colour object.
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
		/// The red value of the colour.
		/// </summary>
		public float R { get; set; }

		/// <summary>
		/// The green value of the colour.
		/// </summary>
		public float G { get; set; }

		/// <summary>
		/// The blue value of the colour.
		/// </summary>
		public float B { get; set; }

		/// <summary>
		/// The alpha value of the colour.
		/// </summary>
		public float A { get; set; }

		#region Statics
		public static Color Red { get { return new Color(1, 0, 0); } }
		public static Color Blue { get { return new Color(0, 0, 1); } }
		public static Color Green { get { return new Color(0, 1, 0); } }

		public static Color Black { get { return new Color(0); } }
		public static Color White { get { return new Color(1); } }
		#endregion
	}
}
