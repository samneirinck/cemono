using System;

namespace CryEngine.Native
{
    internal interface INativePhysicsMethods
    {
        IntPtr GetPhysicalEntity(IntPtr entityPointer);
        int RayWorldIntersection(Vec3 origin, Vec3 dir, EntityQueryFlags objFlags, RayWorldIntersectionFlags flags, ref RayHit rayHit, int maxHits, object[] skipEnts);
        void Physicalize(IntPtr entPtr, PhysicalizationParams physicalizationParams, PlayerPhysicalizationParams playerPhysicalizationParams);
        void Sleep(IntPtr entPtr, bool sleep);
        void AddImpulse(IntPtr entPtr, ActionImpulse actionImpulse);
        Vec3 GetVelocity(IntPtr entPtr);
        void SetVelocity(IntPtr entPtr, Vec3 velocity);
    }
}
