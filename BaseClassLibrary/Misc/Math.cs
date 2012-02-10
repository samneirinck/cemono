namespace CryEngine
{
	public static class Math
	{
		public static double DegToRad(double angle)
		{
			return System.Math.PI * angle / 180.0;
		}

		public static double RadToDeg(double angle)
		{
			return angle * (180.0 / System.Math.PI);
		}
	}
}
