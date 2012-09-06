using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using CryEngine.Initialization;
using CryEngine.Native;

namespace CryEngine
{
	/// <summary>
	/// Contains common entity functionality.
	/// </summary>
	public abstract class EntityBase : CryScriptInstance
	{
		#region Callbacks
		/// <summary>
		/// This callback is called when this entity has finished spawning. The entity has been created and added to the list of entities.
		/// </summary>
		public virtual void OnSpawn() { }
		#endregion

		internal void SetEntityHandle(IntPtr ptr)
		{
			EntityHandleRef = new HandleRef(this, ptr);
		}

		public EntitySlotFlags GetSlotFlags(int slot = 0)
		{
			return NativeMethods.Entity.GetSlotFlags(EntityHandleRef.Handle, slot);
		}

		public void SetSlotFlags(EntitySlotFlags flags, int slot = 0)
		{
			NativeMethods.Entity.SetSlotFlags(EntityHandleRef.Handle, slot, flags);
		}

		public Vec3 Velocity
		{
			get { return NativeMethods.Physics.GetVelocity(EntityHandleRef.Handle); }

			set { NativeMethods.Physics.SetVelocity(EntityHandleRef.Handle, value); }
		}

		EntityPhysics _physics;
		public EntityPhysics Physics
		{
			get { return _physics ?? (_physics = new EntityPhysics(this)); }
		}

        internal override void OnScriptReloadInternal()
		{
            EntityHandleRef = new HandleRef(this, NativeMethods.Entity.GetEntity(Id));

			Physics.OnScriptReload();
            base.OnScriptReloadInternal();
		}

		// TODO: Expose the attachment system properly
#region Attachments
		public Material GetAttachmentMaterial(int index)
		{
			var ptr = NativeMethods.Entity.GetAttachmentMaterialByIndex(EntityHandleRef.Handle, index);

			return Material.TryAdd(ptr);
		}

		public Material GetAttachmentMaterial(string attachmentName)
		{
			var ptr = NativeMethods.Entity.GetAttachmentMaterial(EntityHandleRef.Handle, attachmentName);

			return Material.TryAdd(ptr);
		}

		public void SetAttachmentMaterial(int index, Material newMaterial)
		{
			NativeMethods.Entity.SetAttachmentMaterialByIndex(EntityHandleRef.Handle, index, newMaterial.HandleRef.Handle);
		}

		public void SetAttachmentMaterial(string name, Material newMaterial)
		{
			NativeMethods.Entity.SetAttachmentMaterial(EntityHandleRef.Handle, name, newMaterial.HandleRef.Handle);
		}

        public int AttachmentCount { get { return NativeMethods.Entity.GetAttachmentCount(EntityHandleRef.Handle); } }
#endregion

		/// <summary>
		/// Links to another entity, becoming the parent. Any change to the parent object is propagated to all child (linked) objects.
		/// </summary>
		/// <param name="linkName"></param>
		/// <param name="otherEntityId"></param>
		/// <param name="relativeRot"></param>
		/// <param name="relativePos"></param>
		/// <returns></returns>
		public bool Link(string linkName, EntityId otherEntityId, Quat relativeRot, Vec3 relativePos)
		{
			return NativeMethods.Entity.AddEntityLink(EntityHandleRef.Handle, linkName, otherEntityId, relativeRot, relativePos);
		}

		/// <summary>
		/// Removes an entity link, see Link.
		/// </summary>
		/// <param name="otherEntityId"></param>
		public void Unlink(EntityId otherEntityId)
		{
			NativeMethods.Entity.RemoveEntityLink(EntityHandleRef.Handle, otherEntityId);
		}

		/// <summary>
		/// Loads a light source to the specified slot, or to the next available slot.
		/// </summary>
		/// <param name="parameters"></param>
		/// <param name="slot"></param>
		/// <returns>The slot where the light source was loaded, or -1 if loading failed.</returns>
		public int LoadLight(LightParams parameters, int slot = 1)
		{
			return NativeMethods.Entity.LoadLight(EntityHandleRef.Handle, slot, parameters);
		}

		/// <summary>
		/// Loads a mesh for this entity. Can optionally load multiple meshes using entity slots.
		/// </summary>
		/// <param name="name"></param>
		/// <param name="slotNumber"></param>
		/// <returns></returns>
		public bool LoadObject(string name, int slotNumber = 0)
		{
			if (name == null)
				throw new ArgumentNullException("name");

			if (name.EndsWith("cgf"))
				NativeMethods.Entity.LoadObject(EntityHandleRef.Handle, name, slotNumber);
			else if (name.EndsWith("cdf") || name.EndsWith("cga") || name.EndsWith("chr"))
				NativeMethods.Entity.LoadCharacter(EntityHandleRef.Handle, name, slotNumber);
			else
				return false;

			return true;
		}

		protected string GetObjectFilePath(int slot = 0)
		{
			return NativeMethods.Entity.GetStaticObjectFilePath(EntityHandleRef.Handle, slot);
		}

		public void PlayAnimation(string animationName, AnimationFlags flags = 0, int slot = 0, int layer = 0, float blend = 0.175f, float speed = 1.0f)
		{
			NativeMethods.Entity.PlayAnimation(EntityHandleRef.Handle, animationName, slot, layer, blend, speed, flags);
		}

		public void FreeSlot(int slot)
		{
			NativeMethods.Entity.FreeSlot(EntityHandleRef.Handle, slot);
		}

		public Lua.ScriptTable ScriptTable { get { return Lua.ScriptTable.Get(EntityHandleRef.Handle); } }

		/// <summary>
		/// Sets / gets the world space entity position.
		/// </summary>
        public Vec3 Position { get { return NativeMethods.Entity.GetWorldPos(EntityHandleRef.Handle); } set { NativeMethods.Entity.SetWorldPos(EntityHandleRef.Handle, value); } }
		/// <summary>
		/// Sets / gets the world space entity orientation quaternion.
		/// </summary>
        public Quat Rotation { get { return NativeMethods.Entity.GetWorldRotation(EntityHandleRef.Handle); } set { NativeMethods.Entity.SetWorldRotation(EntityHandleRef.Handle, value); } }

		/// <summary>
		/// Sets / gets the local space entity position.
		/// </summary>
        public Vec3 LocalPosition { get { return NativeMethods.Entity.GetPos(EntityHandleRef.Handle); } set { NativeMethods.Entity.SetPos(EntityHandleRef.Handle, value); } }
		/// <summary>
		/// Sets / gets the local space entity orientation quaternion.
		/// </summary>
        public Quat LocalRotation { get { return NativeMethods.Entity.GetRotation(EntityHandleRef.Handle); } set { NativeMethods.Entity.SetRotation(EntityHandleRef.Handle, value); } }

		/// <summary>
		/// Sets / gets the world space entity transformation matrix.
		/// </summary>
        public Matrix34 Transform { get { return NativeMethods.Entity.GetWorldTM(EntityHandleRef.Handle); } set { NativeMethods.Entity.SetWorldTM(EntityHandleRef.Handle, value); } }
		/// <summary>
		/// Sets / gets the local space entity transformation matrix.
		/// </summary>
        public Matrix34 LocalTransform { get { return NativeMethods.Entity.GetLocalTM(EntityHandleRef.Handle); } set { NativeMethods.Entity.SetLocalTM(EntityHandleRef.Handle, value); } }

		/// <summary>
		/// Gets the entity axis aligned bounding box in the world space.
		/// </summary>
		public BoundingBox BoundingBox { get { return NativeMethods.Entity.GetWorldBoundingBox(EntityHandleRef.Handle); } }
		/// <summary>
		/// Gets the entity axis aligned bounding box in the world space.
		/// </summary>
		public BoundingBox LocalBoundingBox { get { return NativeMethods.Entity.GetBoundingBox(EntityHandleRef.Handle); } }

		/// <summary>
		/// Gets / sets the entity name.
		/// </summary>
		public string Name { get { return NativeMethods.Entity.GetName(EntityHandleRef.Handle); } set { NativeMethods.Entity.SetName(EntityHandleRef.Handle, value); } }
		/// <summary>
		/// Gets / sets the entity flags.
		/// </summary>
		public EntityFlags Flags { get { return NativeMethods.Entity.GetFlags(EntityHandleRef.Handle); } set { NativeMethods.Entity.SetFlags(EntityHandleRef.Handle, value); } }

		public Material Material { get { return Material.Get(this); } set { Material.Set(this, value); } }

		public HandleRef EntityHandleRef { get; private set; }
		public EntityId Id { get; set; }
	}
}