using System.Runtime.CompilerServices;

using CryEngine;

namespace CryEngine.Utils
{
	public static class Debug
	{
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _AddPersistentSphere(Vec3 pos, float radius, Color color, float timeout);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _AddDirection(Vec3 pos, float radius, Vec3 dir, Color color, float timeout);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _AddPersistentText2D(string text, float size, Color color, float timeout);

		public static void AddPersistentSphere(Vec3 pos, float radius, Color color, float timeout)
		{
			_AddPersistentSphere(pos, radius, color, timeout);
		}

		public static void AddDirection(Vec3 pos, float radius, Vec3 dir, Color color, float timeout)
		{
			_AddDirection(pos, radius, dir, color, timeout);
		}

		public static void AddPersistentText2D(string text, float size, Color color, float timeout)
		{
			_AddPersistentText2D(text, size, color, timeout);
		}
	}
}
