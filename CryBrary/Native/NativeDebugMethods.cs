using System.Runtime.CompilerServices;

namespace CryEngine.Native
{
    internal class NativeDebugMethods : INativeDebugMethods
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void _AddPersistentSphere(Vec3 pos, float radius, Color color, float timeout);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void _AddDirection(Vec3 pos, float radius, Vec3 dir, Color color, float timeout);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void _AddPersistentText2D(string text, float size, Color color, float timeout);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void _AddPersistentLine(Vec3 pos, Vec3 end, Color color, float timeout);


        public void AddPersistentSphere(Vec3 pos, float radius, Color color, float timeout)
        {
            _AddPersistentSphere(pos,radius,color,timeout);
        }

        public void AddDirection(Vec3 pos, float radius, Vec3 dir, Color color, float timeout)
        {
            _AddDirection(pos,radius,dir,color,timeout);
        }

        public void AddPersistentText2D(string text, float size, Color color, float timeout)
        {
            _AddPersistentText2D(text, size, color, timeout);
        }

        public void AddPersistentLine(Vec3 pos, Vec3 end, Color color, float timeout)
        {
            _AddPersistentLine(pos, end, color, timeout);
        }
    }
}
