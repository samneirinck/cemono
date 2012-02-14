namespace CryEngine
{
	public static class Math
	{
		/// <summary>
		/// Converts degrees to radians
		/// </summary>
		/// <param name="angle"></param>
		/// <returns></returns>
		public static double DegToRad(double angle)
		{
			return System.Math.PI * angle / 180.0;
		}

		/// <summary>
		/// Converts radians to degrees
		/// </summary>
		/// <param name="angle"></param>
		/// <returns></returns>
		public static double RadToDeg(double angle)
		{
			return angle * (180.0 / System.Math.PI);
		}

		//
		// Summary:
		//     Returns the square root of a specified number.
		//
		// Parameters:
		//   d:
		//     A number.
		//
		// Returns:
		//     One of the values in the following table. d parameter Return value Zero,
		//     or positive The positive square root of d. Negative System.Double.NaNEquals
		//     System.Double.NaNSystem.Double.NaNEquals System.Double.PositiveInfinitySystem.Double.PositiveInfinity
		public static double Sqrt(double d)
		{
			return System.Math.Sqrt(d);
		}

		public static double ISqrt(double d)
		{
			return 1.0 / Sqrt(d);
		}

		public static double Sin(double a)
		{
			return System.Math.Sin(a);
		}

		public static void SinCos(double a, out double sinVal, out double cosVal)
		{
			sinVal = Sin(a);

			cosVal = Sqrt(1.0 - sinVal * sinVal);
		}
	}
}
