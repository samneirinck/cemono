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
		/// The red value of the colour.
		/// </summary>
		public float R { get { return r; } }

		/// <summary>
		/// The blue value of the colour.
		/// </summary>
		public float B { get { return b; } }

		/// <summary>
		/// The green value of the colour.
		/// </summary>
		public float G { get { return g; } }

		/// <summary>
		/// The alpha value of the colour.
		/// </summary>
		public float A { get { return a; } }

		/// <summary>
		/// Constructs a new colour object specifying the red, green, blue and alpha values.
		/// </summary>
		/// <param name="r"></param>
		/// <param name="g"></param>
		/// <param name="b"></param>
		/// <param name="alpha"></param>
		public Color(float r, float g, float b, float alpha)
			: this()
		{
			if(!Math.IsInRange(r, 0, 1) || !Math.IsInRange(g, 0, 1) || !Math.IsInRange(b, 0, 1) || !Math.IsInRange(a, 0, 1))
			{
				throw new ArgumentException("Color and alpha values must be between 0 and 1.");
			}

			this.r = r;
			this.g = g;
			this.b = b;
			this.a = alpha;
		}

		/// <summary>
		/// Constructs a new colour object specifying the red, green, and blue values.
		/// </summary>
		/// <param name="r"></param>
		/// <param name="g"></param>
		/// <param name="b"></param>
		public Color(float r, float g, float b) : this(r, g, b, 1) { }

		/// <summary>
		/// Constructs a new greyscale colour object.
		/// </summary>
		/// <param name="brightness">The brightness of the object, where 0 is black, and 1 is white.</param>
		public Color(float brightness) : this(brightness, brightness, brightness) { }

		public override string ToString()
		{
			return String.Format("R {0} G {1} B {2} A {3}", r, g, b, a);
		}

		// [0.0, 1.0]
		float r, g, b, a;

		public static Color Red { get { return new Color(1, 0, 0); } }
		public static Color Blue { get { return new Color(0, 0, 1); } }
		public static Color Green { get { return new Color(0, 1, 0); } }

		public static Color Black { get { return new Color(0); } }
		public static Color White { get { return new Color(1); } }
	}
}
