using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CryEngine
{
	public struct Color
	{
		public Color(float r, float g, float b)
			: this()
		{
			this.r = r;
			this.g = g;
			this.b = b;
			this.a = 1.0f;
		}

		public Color(float r, float g, float b, float alpha)
			: this()
		{
			this.r = r;
			this.g = g;
			this.b = b;
			this.a = alpha;
		}

		public Color(EColor clr)
			: this()
		{
			this.a = 1.0f;

			switch (clr)
			{
				case EColor.Red:
					this.r = 1;
					break;
				case EColor.Green:
					this.g = 1;
					break;
				case EColor.Blue:
					this.b = 1;
					break;
			}
		}

		public override string ToString()
		{
			return String.Format("R {0} G {1} B {2} A {3}", r, g, b, a);
		}

		// [0.0, 1.0]
		float r, g, b, a;
	}

	public enum EColor
	{
		Red,
		Blue,
		Green
	}
}
