using System;
using System.Runtime.CompilerServices;

namespace CryEngine.Native
{
    internal class NativePhysicsMethods : INativePhysicsMethods
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static IntPtr _GetPhysicalEntity(IntPtr entityPointer);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static int _RayWorldIntersection(Vec3 origin, Vec3 dir, EntityQueryFlags objFlags, RayWorldIntersectionFlags flags, ref RayHit rayHit, int maxHits, object[] skipEnts);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void _Physicalize(IntPtr entPtr, PhysicalizationParams physicalizationParams);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void _Sleep(IntPtr entPtr, bool sleep);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void _AddImpulse(IntPtr entPtr, ActionImpulse actionImpulse);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static Vec3 _GetVelocity(IntPtr entPtr);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void _SetVelocity(IntPtr entPtr, Vec3 velocity);

        public IntPtr GetPhysicalEntity(IntPtr entityPointer)
        {
            return _GetPhysicalEntity(entityPointer);
        }

        public int RayWorldIntersection(Vec3 origin, Vec3 dir, EntityQueryFlags objFlags, RayWorldIntersectionFlags flags, ref RayHit rayHit, int maxHits, object[] skipEnts)
        {
            return _RayWorldIntersection(origin, dir, objFlags, flags, ref rayHit, maxHits, skipEnts);
        }

        public void Physicalize(IntPtr entPtr, PhysicalizationParams physicalizationParams)
        {
            _Physicalize(entPtr, physicalizationParams);
        }

        public void Sleep(IntPtr entPtr, bool sleep)
        {
            _Sleep(entPtr, sleep);
        }

        public void AddImpulse(IntPtr entPtr, ActionImpulse actionImpulse)
        {
            _AddImpulse(entPtr, actionImpulse);
        }

        public Vec3 GetVelocity(IntPtr entPtr)
        {
            return _GetVelocity(entPtr);
        }

        public void SetVelocity(IntPtr entPtr, Vec3 velocity)
        {
            _SetVelocity(entPtr, velocity);
        }
    }
}
