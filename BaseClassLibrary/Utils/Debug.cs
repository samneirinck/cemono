using System.Runtime.CompilerServices;

using CryEngine;

namespace CryEngine.Utils
{
	public static class Debug
	{
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _AddPersistantSphere(Vec3 pos, float radius, Color color, float timeout);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _AddDirection(Vec3 pos, float radius, Vec3 dir, Color color, float timeout);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _AddPersistantText2D(string text, float size, Color color, float timeout);

		public static void AddPersistantSphere(Vec3 pos, float radius, Color color, float timeout)
		{
			_AddPersistantSphere(pos, radius, color, timeout);
		}

		public static void AddDirection(Vec3 pos, float radius, Vec3 dir, Color color, float timeout)
		{
			_AddDirection(pos, radius, dir, color, timeout);
		}

		public static void AddPersistantText2D(string text, float size, Color color, float timeout)
		{
			_AddPersistantText2D(text, size, color, timeout);
		}
	}
}
