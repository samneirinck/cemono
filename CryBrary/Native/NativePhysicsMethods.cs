using System;
using System.Runtime.CompilerServices;

namespace CryEngine.Native
{
    internal class NativePhysicsMethods : INativePhysicsMethods
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static IntPtr _GetPhysicalEntity(IntPtr entityPointer);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static int _RayWorldIntersection(Vec3 origin, Vec3 dir, EntityQueryFlags objFlags, RayWorldIntersectionFlags flags, out RaycastHit rayHit, int maxHits, object[] skipEnts);

        [MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _Physicalize(IntPtr entPtr, PhysicalizationParams physicalizationParams);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void _Sleep(IntPtr entPtr, bool sleep);

        [MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _AddImpulse(IntPtr entPtr, pe_action_impulse actionImpulse);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static Vec3 _GetVelocity(IntPtr entPtr);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void _SetVelocity(IntPtr entPtr, Vec3 velocity);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _SimulateExplosion(ref Explosion explosion);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static pe_status_living _GetLivingEntityStatus(IntPtr entPtr);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static pe_action_impulse _GetImpulseStruct();
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static pe_player_dimensions _GetPlayerDimensionsStruct();
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static pe_player_dynamics _GetPlayerDynamicsStruct();

        public IntPtr GetPhysicalEntity(IntPtr entityPointer)
        {
            return _GetPhysicalEntity(entityPointer);
        }

		public int RayWorldIntersection(Vec3 origin, Vec3 dir, EntityQueryFlags objFlags, RayWorldIntersectionFlags flags, out RaycastHit rayHit, int maxHits, object[] skipEnts)
        {
            return _RayWorldIntersection(origin, dir, objFlags, flags, out rayHit, maxHits, skipEnts);
        }

		public void Physicalize(IntPtr entPtr, PhysicalizationParams physicalizationParams)
        {
			_Physicalize(entPtr, physicalizationParams);
        }

        public void Sleep(IntPtr entPtr, bool sleep)
        {
            _Sleep(entPtr, sleep);
        }

		public void AddImpulse(IntPtr entPtr, pe_action_impulse actionImpulse)
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

		public void SimulateExplosion(ref Explosion explosion)
		{
			_SimulateExplosion(ref explosion);
		}

		public pe_status_living GetLivingEntityStatus(IntPtr entPtr)
		{
			return _GetLivingEntityStatus(entPtr);
		}

		public pe_action_impulse GetImpulseStruct()
		{
			return _GetImpulseStruct();
		}

		public pe_player_dimensions GetPlayerDimensionsStruct()
		{
			return _GetPlayerDimensionsStruct();
		}

		public pe_player_dynamics GetPlayerDynamicsStruct()
		{
			return _GetPlayerDynamicsStruct();
		}
    }
}
