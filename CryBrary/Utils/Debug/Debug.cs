using System.Runtime.CompilerServices;


namespace CryEngine
{
	public static partial class Debug
	{
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _AddPersistentSphere(Vec3 pos, float radius, Color color, float timeout);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _AddDirection(Vec3 pos, float radius, Vec3 dir, Color color, float timeout);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _AddPersistentText2D(string text, float size, Color color, float timeout);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _AddPersistentLine(Vec3 pos, Vec3 end, Color color, float timeout);

		public static void DrawSphere(Vec3 pos, float radius, Color color, float timeout)
		{
			_AddPersistentSphere(pos, radius, color, timeout);
		}

		public static void DrawDirection(Vec3 pos, float radius, Vec3 dir, Color color, float timeout)
		{
			_AddDirection(pos, radius, dir, color, timeout);
		}

		public static void DrawText(string text, float size, Color color, float timeout)
		{
			_AddPersistentText2D(text, size, color, timeout);
		}

		public static void DrawLine(Vec3 startPos, Vec3 endPos, Color color, float timeout)
		{
			_AddPersistentLine(startPos, endPos, color, timeout);
		}
	}
}
