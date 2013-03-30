using System;
using System.Runtime.CompilerServices;

using CryEngine.Physics;
using CryEngine.Physics.Actions;
using CryEngine.Physics.Status;

namespace CryEngine.Native
{
    public static class NativePhysicsMethods
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern public static IntPtr GetPhysicalEntity(IntPtr entityPointer);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern public static PhysicalizationType GetPhysicalEntityType(IntPtr physicalEntPtr);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static int RayWorldIntersection(Vec3 origin, Vec3 dir, EntityQueryFlags objFlags, RayWorldIntersectionFlags flags, int maxHits, IntPtr[] skipEnts, out object[] hits);

		[CLSCompliant(false)]
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern public static void Physicalize(IntPtr entPtr, PhysicalizationParams physicalizationParams);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern public static void Sleep(IntPtr entPtr, bool sleep);

        [MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void AddImpulse(IntPtr entPtr, pe_action_impulse actionImpulse);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern public static Vec3 GetVelocity(IntPtr entPtr);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern public static void SetVelocity(IntPtr entPtr, Vec3 velocity);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static object[] SimulateExplosion(pe_explosion explosion);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern public static LivingPhysicsStatus GetLivingEntityStatus(IntPtr entPtr);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern public static DynamicsPhysicsStatus GetDynamicsEntityStatus(IntPtr entPtr);

		[CLSCompliant(false)]
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern public static bool SetParticleParams(IntPtr physEnt, ref ParticleParameters parameters);
		[CLSCompliant(false)]
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern public static bool GetParticleParams(IntPtr physEnt, ref ParticleParameters parameters);

		[CLSCompliant(false)]
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern public static bool SetFlagParams(IntPtr physEnt, ref PhysicalFlagsParameters parameters);
		[CLSCompliant(false)]
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern public static bool GetFlagParams(IntPtr physEnt, ref PhysicalFlagsParameters parameters);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern public static bool SetSimulationParams(IntPtr physEnt, ref PhysicalSimulationParameters parameters);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern public static bool GetSimulationParams(IntPtr physEnt, ref PhysicalSimulationParameters parameters);
    }
}
