using System;
using CryEngine.Initialization;

namespace CryEngine.Native
{
    internal interface INativeEntityMethods
    {
		void PlayAnimation(IntPtr ptr, string animationName, int slot, int layer, float blend, float speed, AnimationFlags flags);

        bool SpawnEntity(EntitySpawnParams spawnParams, bool autoInit, out EntityInfo entityInfo);
		void RemoveEntity(EntityId entityId);

		IntPtr GetEntity(EntityId entityId);
        uint FindEntity(string name);
        object[] GetEntitiesByClass(string className);
        object[] GetEntitiesInBox(BoundingBox bbox, EntityQueryFlags flags);

        bool RegisterEntityClass(EntityRegistrationParams registerParams);

        void BreakIntoPieces(IntPtr ptr, int slot, int piecesSlot, BreakageParameters breakageParams);

        string GetName(IntPtr ptr);
        void SetName(IntPtr ptr, string name);

        EntityFlags GetFlags(IntPtr ptr);
        void SetFlags(IntPtr ptr, EntityFlags name);

        void AddMovement(IntPtr ptr, ref EntityMovementRequest request);

        void SetWorldTM(IntPtr ptr, Matrix34 tm);
        Matrix34 GetWorldTM(IntPtr ptr);
        void SetLocalTM(IntPtr ptr, Matrix34 tm);
        Matrix34 GetLocalTM(IntPtr ptr);

        BoundingBox GetWorldBoundingBox(IntPtr ptr);
        BoundingBox GetBoundingBox(IntPtr ptr);

        EntitySlotFlags GetSlotFlags(IntPtr ptr, int slot);
        void SetSlotFlags(IntPtr ptr, int slot, EntitySlotFlags slotFlags);

        void SetWorldPos(IntPtr ptr, Vec3 newPos);
        Vec3 GetWorldPos(IntPtr ptr);
        void SetPos(IntPtr ptr, Vec3 newPos);
        Vec3 GetPos(IntPtr ptr);

        void SetWorldRotation(IntPtr ptr, Quat newAngles);
        Quat GetWorldRotation(IntPtr ptr);
        void SetRotation(IntPtr ptr, Quat newAngles);
        Quat GetRotation(IntPtr ptr);

        void LoadObject(IntPtr ptr, string fileName, int slot);
        string GetStaticObjectFilePath(IntPtr ptr, int slot);

        void LoadCharacter(IntPtr ptr, string fileName, int slot);

        int GetAttachmentCount(IntPtr entPtr);
        IntPtr GetAttachmentMaterialByIndex(IntPtr entPtr, int index);
        void SetAttachmentMaterialByIndex(IntPtr entPtr, int index, IntPtr materialPtr);

        IntPtr GetAttachmentMaterial(IntPtr entPtr, string name);
        int SetAttachmentMaterial(IntPtr entPtr, string name, IntPtr materialPtr);

        void RegisterClass(EntityRegistrationParams registrationParams);

		bool AddEntityLink(IntPtr entPtr, string linkName, EntityId otherId, Quat relativeRot, Vec3 relativePos);
		void RemoveEntityLink(IntPtr entPtr, EntityId otherId);

		int LoadLight(IntPtr entPtr, int slot, LightParams lightParams);
		void FreeSlot(IntPtr entPtr, int slot);
    }
}