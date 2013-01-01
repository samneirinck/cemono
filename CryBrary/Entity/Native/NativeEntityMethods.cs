using System;
using System.Runtime.CompilerServices;

using CryEngine.Initialization;
using CryEngine.Physics;

namespace CryEngine.Native
{
    internal static class NativeEntityMethods
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void PlayAnimation(IntPtr ptr, string animationName, int slot, int layer, float blend, float speed, AnimationFlags flags);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void StopAnimationInLayer(IntPtr ptr, int slot, int layer, float blendOutTime);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void StopAnimationsInAllLayers(IntPtr ptr, int slot);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static EntityBase SpawnEntity(EntitySpawnParams spawnParams, bool autoInit, out EntityInfo entityInfo);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void RemoveEntity(uint entityId, bool forceRemoveNow = false);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static IntPtr GetEntity(uint entityId);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static EntityId GetEntityId(IntPtr entPtr);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static uint FindEntity(string name);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static object[] GetEntitiesByClass(string className);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static object[] GetEntitiesInBox(BoundingBox bbox, EntityQueryFlags flags);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static object[] QueryProximity(BoundingBox box, string className, EntityFlags flags);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static bool RegisterEntityClass(EntityRegistrationParams registerParams);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void BreakIntoPieces(IntPtr ptr, int slot, int piecesSlot, BreakageParameters breakageParams);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static string GetName(IntPtr ptr);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void SetName(IntPtr ptr, string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static EntityFlags GetFlags(IntPtr ptr);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void SetFlags(IntPtr ptr, EntityFlags name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void AddMovement(IntPtr animatedCharacterPtr, ref EntityMovementRequest request);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void SetWorldTM(IntPtr ptr, Matrix34 tm);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static Matrix34 GetWorldTM(IntPtr ptr);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void SetLocalTM(IntPtr ptr, Matrix34 tm);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static Matrix34 GetLocalTM(IntPtr ptr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static BoundingBox GetWorldBoundingBox(IntPtr ptr);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static BoundingBox GetBoundingBox(IntPtr ptr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static EntitySlotFlags GetSlotFlags(IntPtr ptr, int slot);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void SetSlotFlags(IntPtr ptr, int slot, EntitySlotFlags slotFlags);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void SetWorldPos(IntPtr ptr, Vec3 newPos);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static Vec3 GetWorldPos(IntPtr ptr);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void SetPos(IntPtr ptr, Vec3 newPos);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static Vec3 GetPos(IntPtr ptr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void SetWorldRotation(IntPtr ptr, Quat newAngles);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static Quat GetWorldRotation(IntPtr ptr);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void SetRotation(IntPtr ptr, Quat newAngles);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static Quat GetRotation(IntPtr ptr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void LoadObject(IntPtr ptr, string fileName, int slot);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static string GetStaticObjectFilePath(IntPtr ptr, int slot);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void LoadCharacter(IntPtr ptr, string fileName, int slot);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static IntPtr AddEntityLink(IntPtr entPtr, string linkName, uint otherId, Quat relativeRot, Vec3 relativePos);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static object[] GetEntityLinks(IntPtr entPtr);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void RemoveAllEntityLinks(IntPtr entPtr);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void RemoveEntityLink(IntPtr entPtr, IntPtr linkPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static string GetEntityLinkName(IntPtr linkPtr);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static EntityId GetEntityLinkTarget(IntPtr linkPtr);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static Quat GetEntityLinkRelativeRotation(IntPtr linkPtr);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static Vec3 GetEntityLinkRelativePosition(IntPtr linkPtr);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void SetEntityLinkTarget(IntPtr linkPtr, EntityId target);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void SetEntityLinkRelativeRotation(IntPtr linkPtr, Quat relRot);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void SetEntityLinkRelativePosition(IntPtr linkPtr, Vec3 relPos);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static int LoadLight(IntPtr entPtr, int slot, LightParams lightParams);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void FreeSlot(IntPtr entPtr, int slot);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static int GetAttachmentCount(IntPtr entPtr, int slot);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static IntPtr GetAttachmentByIndex(IntPtr entPtr, int index, int slot);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static IntPtr GetAttachmentByName(IntPtr entPtr, string name, int slot);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void AttachmentUseEntityPosition(IntPtr entityAttachmentPtr, bool use);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void AttachmentUseEntityRotation(IntPtr entityAttachmentPtr, bool use);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static IntPtr LinkEntityToAttachment(IntPtr attachmentPtr, uint entityId);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static string GetAttachmentObject(IntPtr attachmentPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static QuatT GetAttachmentAbsolute(IntPtr attachmentPtr);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static QuatT GetAttachmentRelative(IntPtr attachmentPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static QuatT GetAttachmentDefaultAbsolute(IntPtr attachmentPtr);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static QuatT GetAttachmentDefaultRelative(IntPtr attachmentPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static IntPtr GetAttachmentMaterial(IntPtr attachmentPtr);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void SetAttachmentMaterial(IntPtr attachmentPtr, IntPtr materialPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static QuatT GetJointAbsolute(IntPtr entPtr, string jointName, int characterSlot);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static QuatT GetJointAbsoluteDefault(IntPtr entPtr, string jointName, int characterSlot);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static QuatT GetJointRelative(IntPtr entPtr, string jointName, int characterSlot);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static QuatT GetJointRelativeDefault(IntPtr entPtr, string jointName, int characterSlot);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void SetJointAbsolute(IntPtr entPtr, string jointName, int characterSlot, QuatT absolute);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void SetTriggerBBox(IntPtr entPtr, BoundingBox bounds);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static BoundingBox GetTriggerBBox(IntPtr entPtr);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void InvalidateTrigger(IntPtr entPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static IntPtr AcquireAnimatedCharacter(uint entId);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void Hide(IntPtr entityId, bool hide);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static bool IsHidden(IntPtr entityId);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static IntPtr GetEntityFromPhysics(IntPtr physEntPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void SetUpdatePolicy(IntPtr entPtr, EntityUpdatePolicy policy);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static EntityUpdatePolicy GetUpdatePolicy(IntPtr entPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static bool IsPrePhysicsUpdateActive(IntPtr entPtr);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void EnablePrePhysicsUpdate(IntPtr entPtr, bool enable);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static IntPtr LoadParticleEmitter(IntPtr entPtr, int slot, IntPtr particlePtr, ref ParticleSpawnParameters spawnParams);
    }
}