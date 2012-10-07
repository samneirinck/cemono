using System;
using System.Runtime.CompilerServices;
using CryEngine.Initialization;

namespace CryEngine.Native
{
    internal class NativeEntityMethods : INativeEntityMethods
    {
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _PlayAnimation(IntPtr ptr, string animationName, int slot, int layer, float blend, float speed, AnimationFlags flags);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static EntityBase _SpawnEntity(EntitySpawnParams spawnParams, bool autoInit, out EntityInfo entityInfo);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _RemoveEntity(uint entityId);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static IntPtr _GetEntity(uint entityId);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static uint _FindEntity(string name);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static object[] _GetEntitiesByClass(string className);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static object[] _GetEntitiesInBox(BoundingBox bbox, EntityQueryFlags flags);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static bool _RegisterEntityClass(EntityRegistrationParams registerParams);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void _BreakIntoPieces(IntPtr ptr, int slot, int piecesSlot, BreakageParameters breakageParams);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static string _GetName(IntPtr ptr);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void _SetName(IntPtr ptr, string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static EntityFlags _GetFlags(IntPtr ptr);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void _SetFlags(IntPtr ptr, EntityFlags name);

        [MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _AddMovement(IntPtr animatedCharacterPtr, ref EntityMovementRequest request);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void _SetWorldTM(IntPtr ptr, Matrix34 tm);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static Matrix34 _GetWorldTM(IntPtr ptr);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void _SetLocalTM(IntPtr ptr, Matrix34 tm);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static Matrix34 _GetLocalTM(IntPtr ptr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static BoundingBox _GetWorldBoundingBox(IntPtr ptr);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static BoundingBox _GetBoundingBox(IntPtr ptr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static EntitySlotFlags _GetSlotFlags(IntPtr ptr, int slot);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void _SetSlotFlags(IntPtr ptr, int slot, EntitySlotFlags slotFlags);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void _SetWorldPos(IntPtr ptr, Vec3 newPos);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static Vec3 _GetWorldPos(IntPtr ptr);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void _SetPos(IntPtr ptr, Vec3 newPos);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static Vec3 _GetPos(IntPtr ptr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void _SetWorldRotation(IntPtr ptr, Quat newAngles);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static Quat _GetWorldRotation(IntPtr ptr);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void _SetRotation(IntPtr ptr, Quat newAngles);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static Quat _GetRotation(IntPtr ptr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void _LoadObject(IntPtr ptr, string fileName, int slot);
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static string _GetStaticObjectFilePath(IntPtr ptr, int slot);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void _LoadCharacter(IntPtr ptr, string fileName, int slot);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static bool _AddEntityLink(IntPtr entPtr, string linkName, uint otherId, Quat relativeRot, Vec3 relativePos);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _RemoveEntityLink(IntPtr entPtr, uint otherId);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static int _LoadLight(IntPtr entPtr, int slot, LightParams lightParams);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _FreeSlot(IntPtr entPtr, int slot);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static int _GetAttachmentCount(IntPtr entPtr, int slot);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static IntPtr _GetAttachmentByIndex(IntPtr entPtr, int index, int slot);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static IntPtr _GetAttachmentByName(IntPtr entPtr, string name, int slot);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _LinkEntityToAttachment(IntPtr attachmentPtr, uint entityId);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static string _GetAttachmentObject(IntPtr attachmentPtr);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static QuatT _GetAttachmentAbsolute(IntPtr attachmentPtr);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static QuatT _GetAttachmentRelative(IntPtr attachmentPtr);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static QuatT _GetAttachmentDefaultAbsolute(IntPtr attachmentPtr);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static QuatT _GetAttachmentDefaultRelative(IntPtr attachmentPtr);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static IntPtr _GetAttachmentMaterial(IntPtr attachmentPtr);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _SetAttachmentMaterial(IntPtr attachmentPtr, IntPtr materialPtr);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static QuatT _GetJointAbsolute(IntPtr entPtr, string jointName, int characterSlot);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static QuatT _GetJointAbsoluteDefault(IntPtr entPtr, string jointName, int characterSlot);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static QuatT _GetJointRelative(IntPtr entPtr, string jointName, int characterSlot);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static QuatT _GetJointRelativeDefault(IntPtr entPtr, string jointName, int characterSlot);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _SetJointAbsolute(IntPtr entPtr, string jointName, int characterSlot, QuatT absolute);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _SetTriggerBBox(IntPtr entPtr, BoundingBox bounds);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static BoundingBox _GetTriggerBBox(IntPtr entPtr);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _InvalidateTrigger(IntPtr entPtr);

		public void PlayAnimation(IntPtr ptr, string animationName, int slot, int layer, float blend, float speed, AnimationFlags flags)
		{
			_PlayAnimation(ptr, animationName, slot, layer, blend, speed, flags);
		}

		public EntityBase SpawnEntity(EntitySpawnParams spawnParams, bool autoInit, out EntityInfo entityInfo)
        {
            return _SpawnEntity(spawnParams, autoInit, out entityInfo);
        }

        public void RemoveEntity(EntityId entityId)
        {
            _RemoveEntity(entityId);
        }

		public IntPtr GetEntity(EntityId entityId)
        {
            return _GetEntity(entityId);
        }

        public uint FindEntity(string name)
        {
            return _FindEntity(name);
        }

        public object[] GetEntitiesByClass(string className)
        {
            return _GetEntitiesByClass(className);
        }

        public object[] GetEntitiesInBox(BoundingBox bbox, EntityQueryFlags flags)
        {
            return _GetEntitiesInBox(bbox, flags);
        }

        public bool RegisterEntityClass(EntityRegistrationParams registerParams)
        {
            return _RegisterEntityClass(registerParams);
        }

        public void BreakIntoPieces(IntPtr ptr, int slot, int piecesSlot, BreakageParameters breakageParams)
        {
            _BreakIntoPieces(ptr,slot,piecesSlot,breakageParams);
        }

        public string GetName(IntPtr ptr)
        {
            return _GetName(ptr);
        }

        public void SetName(IntPtr ptr, string name)
        {
            _SetName(ptr,name);
        }

        public EntityFlags GetFlags(IntPtr ptr)
        {
            return _GetFlags(ptr);
        }

        public void SetFlags(IntPtr ptr, EntityFlags name)
        {
            _SetFlags(ptr, name);
        }

        public void AddMovement(IntPtr ptr, ref EntityMovementRequest request)
        {
            _AddMovement(ptr, ref request);
        }

        public void SetWorldTM(IntPtr ptr, Matrix34 tm)
        {
            _SetWorldTM(ptr,tm);
        }

        public Matrix34 GetWorldTM(IntPtr ptr)
        {
            return _GetWorldTM(ptr);
        }

        public void SetLocalTM(IntPtr ptr, Matrix34 tm)
        {
            _SetLocalTM(ptr,tm);
        }

        public Matrix34 GetLocalTM(IntPtr ptr)
        {
            return _GetLocalTM(ptr);
        }

        public BoundingBox GetWorldBoundingBox(IntPtr ptr)
        {
            return _GetWorldBoundingBox(ptr);
        }

        public BoundingBox GetBoundingBox(IntPtr ptr)
        {
            return _GetBoundingBox(ptr);
        }

        public EntitySlotFlags GetSlotFlags(IntPtr ptr, int slot)
        {
            return _GetSlotFlags(ptr, slot);
        }

        public void SetSlotFlags(IntPtr ptr, int slot, EntitySlotFlags slotFlags)
        {
            _SetSlotFlags(ptr,slot,slotFlags);
        }

        public void SetWorldPos(IntPtr ptr, Vec3 newPos)
        {
            _SetWorldPos(ptr, newPos);
        }

        public Vec3 GetWorldPos(IntPtr ptr)
        {
            return _GetWorldPos(ptr);
        }

        public void SetPos(IntPtr ptr, Vec3 newPos)
        {
            _SetPos(ptr,newPos);
        }

        public Vec3 GetPos(IntPtr ptr)
        {
            return _GetPos(ptr);
        }

        public void SetWorldRotation(IntPtr ptr, Quat newAngles)
        {
            _SetWorldRotation(ptr,newAngles);
        }

        public Quat GetWorldRotation(IntPtr ptr)
        {
            return _GetWorldRotation(ptr);
        }

        public void SetRotation(IntPtr ptr, Quat newAngles)
        {
            _SetRotation(ptr,newAngles);
        }

        public Quat GetRotation(IntPtr ptr)
        {
            return _GetRotation(ptr);
        }

        public void LoadObject(IntPtr ptr, string fileName, int slot)
        {
            _LoadObject(ptr,fileName,slot);
        }

        public string GetStaticObjectFilePath(IntPtr ptr, int slot)
        {
            return _GetStaticObjectFilePath(ptr, slot);
        }

        public void LoadCharacter(IntPtr ptr, string fileName, int slot)
        {
            _LoadCharacter(ptr,fileName,slot);
        }

        public void RegisterClass(EntityRegistrationParams registrationParams)
        {
            _RegisterEntityClass(registrationParams);
        }

		public bool AddEntityLink(IntPtr entPtr, string linkName, EntityId otherId, Quat relativeRot, Vec3 relativePos)
		{
			return _AddEntityLink(entPtr, linkName, otherId, relativeRot, relativePos);
		}

		public void RemoveEntityLink(IntPtr entPtr, EntityId otherId)
		{
			_RemoveEntityLink(entPtr, otherId);
		}

		public int LoadLight(IntPtr entPtr, int slot, LightParams lightParams)
		{
			return _LoadLight(entPtr, slot, lightParams);
		}

		public void FreeSlot(IntPtr entPtr, int slot)
		{
			_FreeSlot(entPtr, slot);
		}

		public int GetAttachmentCount(IntPtr entPtr, int slot)
		{
			return _GetAttachmentCount(entPtr, slot);
		}

		public IntPtr GetAttachmentByIndex(IntPtr entPtr, int index, int slot)
		{
			return _GetAttachmentByIndex(entPtr, index, slot);
		}

		public IntPtr GetAttachmentByName(IntPtr entPtr, string name, int slot)
		{
			return _GetAttachmentByName(entPtr, name, slot);
		}

		public void LinkEntityToAttachment(IntPtr attachmentPtr, uint entityId)
		{
			_LinkEntityToAttachment(attachmentPtr, entityId);
		}

		public string GetAttachmentObject(IntPtr attachmentPtr)
		{
			return _GetAttachmentObject(attachmentPtr);
		}

		public QuatT GetAttachmentAbsolute(IntPtr attachmentPtr)
		{
			return _GetAttachmentAbsolute(attachmentPtr);
		}

		public QuatT GetAttachmentRelative(IntPtr attachmentPtr)
		{
			return _GetAttachmentRelative(attachmentPtr);
		}

		public QuatT GetAttachmentDefaultAbsolute(IntPtr attachmentPtr)
		{
			return _GetAttachmentDefaultAbsolute(attachmentPtr);
		}

		public QuatT GetAttachmentDefaultRelative(IntPtr attachmentPtr)
		{
			return _GetAttachmentDefaultRelative(attachmentPtr);
		}

		public IntPtr GetAttachmentMaterial(IntPtr attachmentPtr)
		{
			return _GetAttachmentMaterial(attachmentPtr); 
		}

		public void SetAttachmentMaterial(IntPtr attachmentPtr, IntPtr materialPtr)
		{
			_SetAttachmentMaterial(attachmentPtr, materialPtr);
		}

		public QuatT GetJointAbsolute(IntPtr entPtr, string jointName, int characterSlot)
		{
			return _GetJointAbsolute(entPtr, jointName, characterSlot);
		}

		public QuatT GetJointAbsoluteDefault(IntPtr entPtr, string jointName, int characterSlot)
		{
			return _GetJointAbsoluteDefault(entPtr, jointName, characterSlot);
		}

		public QuatT GetJointRelative(IntPtr entPtr, string jointName, int characterSlot)
		{
			return _GetJointRelative(entPtr, jointName, characterSlot);
		}

		public QuatT GetJointRelativeDefault(IntPtr entPtr, string jointName, int characterSlot)
		{
			return _GetJointRelativeDefault(entPtr, jointName, characterSlot);
		}

		public void SetJointAbsolute(IntPtr entPtr, string jointName, int characterSlot, QuatT absolute)
		{
			_SetJointAbsolute(entPtr, jointName, characterSlot, absolute);
		}

		public void SetTriggerBBox(IntPtr entPtr, BoundingBox bounds)
		{
			_SetTriggerBBox(entPtr, bounds);
		}

		public BoundingBox GetTriggerBBox(IntPtr entPtr)
		{
			return _GetTriggerBBox(entPtr);
		}

		public void InvalidateTrigger(IntPtr entPtr)
		{
			_InvalidateTrigger(entPtr);
		}
    }
}
