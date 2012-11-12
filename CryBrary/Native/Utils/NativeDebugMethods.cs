using System.Runtime.CompilerServices;

namespace CryEngine.Native
{
    internal class NativeDebugMethods
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void AddPersistentSphere(Vec3 pos, float radius, Color color, float timeout);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void AddDirection(Vec3 pos, float radius, Vec3 dir, Color color, float timeout);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void AddPersistentText2D(string text, float size, Color color, float timeout);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void AddPersistentLine(Vec3 pos, Vec3 end, Color color, float timeout);
    }
}
