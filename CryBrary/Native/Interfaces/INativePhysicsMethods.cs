using System;

namespace CryEngine.Native
{
    internal interface INativePhysicsMethods
    {
        IntPtr GetPhysicalEntity(IntPtr entityPointer);
        int RayWorldIntersection(Vec3 origin, Vec3 dir, EntityQueryFlags objFlags, RayWorldIntersectionFlags flags, ref RayHit rayHit, int maxHits, object[] skipEnts);
        void Physicalize(IntPtr entPtr, PhysicalizationParams physicalizationParams);
        void Sleep(IntPtr entPtr, bool sleep);
        void AddImpulse(IntPtr entPtr, pe_action_impulse actionImpulse);
        Vec3 GetVelocity(IntPtr entPtr);
        void SetVelocity(IntPtr entPtr, Vec3 velocity);

		pe_action_impulse GetImpulseStruct();
		pe_player_dimensions GetPlayerDimensionsStruct();
		pe_player_dynamics GetPlayerDynamicsStruct();
    }
}
