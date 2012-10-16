using System.Runtime.CompilerServices;
using CryEngine.Native;


namespace CryEngine
{
	/// <summary>
	/// Contains methods useful for tracking down bugs.
	/// </summary>
	public static partial class Debug
	{
        public static void DrawSphere(Vec3 pos, float radius, Color color, float timeout)
		{
            NativeMethods.Debug.AddPersistentSphere(pos, radius, color, timeout);
		}

		public static void DrawDirection(Vec3 pos, float radius, Vec3 dir, Color color, float timeout)
		{
            NativeMethods.Debug.AddDirection(pos, radius, dir, color, timeout);
		}

		public static void DrawText(string text, float size, Color color, float timeout)
		{
            NativeMethods.Debug.AddPersistentText2D(text, size, color, timeout);
		}

		public static void DrawLine(Vec3 startPos, Vec3 endPos, Color color, float timeout)
		{
            NativeMethods.Debug.AddPersistentLine(startPos, endPos, color, timeout);
		}
	}
}
