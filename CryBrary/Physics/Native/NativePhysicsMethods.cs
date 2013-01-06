using System;
using System.Runtime.CompilerServices;

using CryEngine.Physics;
using CryEngine.Physics.Actions;
using CryEngine.Physics.Status;

namespace CryEngine.Native
{
    internal static class NativePhysicsMethods
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static IntPtr GetPhysicalEntity(IntPtr entityPointer);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static int RayWorldIntersection(Vec3 origin, Vec3 dir, EntityQueryFlags objFlags, RayWorldIntersectionFlags flags, int maxHits, IntPtr[] skipEnts, out object[] hits);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void Physicalize(IntPtr entPtr, PhysicalizationParams physicalizationParams);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void Sleep(IntPtr entPtr, bool sleep);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void AddImpulse(IntPtr entPtr, pe_action_impulse actionImpulse);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static Vec3 GetVelocity(IntPtr entPtr);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void SetVelocity(IntPtr entPtr, Vec3 velocity);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static object[] SimulateExplosion(pe_explosion explosion);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static LivingPhysicsStatus GetLivingEntityStatus(IntPtr entPtr);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static DynamicsPhysicsStatus GetDynamicsEntityStatus(IntPtr entPtr);
    }
}
