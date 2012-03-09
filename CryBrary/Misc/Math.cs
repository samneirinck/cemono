using System;

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

		/// <summary>
		/// Returns the square root of a specified number.
		/// </summary>
		/// <param name="d"></param>
		/// <returns></returns>
		public static double Sqrt(double d)
		{
			return System.Math.Sqrt(d);
		}

		public static double ISqrt(double d)
		{
			return 1.0 / Sqrt(d);
		}

		public static double Abs(double d)
		{
			return System.Math.Abs(d);
		}

		public static double Sin(double a)
		{
			return System.Math.Sin(a);
		}

		public static double Asin(double d)
		{
			return System.Math.Asin(d);
		}

		public static double Cos(double d)
		{
			return System.Math.Cos(d);
		}

		public static double Acos(double d)
		{
			return System.Math.Acos(d);
		}

		public static double Tan(double d)
		{
			return System.Math.Tan(d);
		}

		public static double Atan(double d)
		{
			return System.Math.Atan(d);
		}

		public static double Atan2(double y, double x)
		{
			return System.Math.Atan2(y, x);
		}

		public static void SinCos(double a, out double sinVal, out double cosVal)
		{
			sinVal = Sin(a);

			cosVal = Sqrt(1.0 - sinVal * sinVal);
		}

		/// <summary>
		/// Determines whether a value is inside the specified range.
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="value"></param>
		/// <param name="min"></param>
		/// <param name="max"></param>
		/// <returns></returns>
		public static bool IsInRange<T>(T value, T min, T max) where T : IComparable<T>
		{
			if(value.CompareTo(min) >= 0 && value.CompareTo(max) <= 0)
				return true;
			else
				return false;
		}

		/// <summary>
		/// Clamps a value given a specified range.
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="value"></param>
		/// <param name="min"></param>
		/// <param name="max"></param>
		/// <returns></returns>
		public static T Clamp<T>(T value, T min, T max) where T : IComparable<T>
		{
			if(value.CompareTo(min) < 0)
				return min;
			else if(value.CompareTo(max) > 0)
				return max;
			else
				return value;
		}

		public static T Max<T>(T val1, T val2) where T : IComparable<T>
		{
			if(val1.CompareTo(val2) > 0)
				return val1;

			return val2;
		}

		public static T Min<T>(T val1, T val2) where T : IComparable<T>
		{
			if(val1.CompareTo(val2) < 0)
				return val1;

			return val2;
		}

		public static double PI { get { return System.Math.PI; } }
	}
}
