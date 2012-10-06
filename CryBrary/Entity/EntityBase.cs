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

		public EntitySlotFlags GetSlotFlags(int slot = 0)
		{
			return NativeMethods.Entity.GetSlotFlags(this.GetEntityHandle().Handle, slot);
		}

		public void SetSlotFlags(EntitySlotFlags flags, int slot = 0)
		{
			NativeMethods.Entity.SetSlotFlags(this.GetEntityHandle().Handle, slot, flags);
		}

		public Vec3 Velocity
		{
			get { return NativeMethods.Physics.GetVelocity(this.GetEntityHandle().Handle); }

			set { NativeMethods.Physics.SetVelocity(this.GetEntityHandle().Handle, value); }
		}

		EntityPhysics _physics;
		public EntityPhysics Physics
		{
			get { return _physics ?? (_physics = new EntityPhysics(this)); }
		}

        internal override void OnScriptReloadInternal()
		{
            this.SetEntityHandle(new HandleRef(this, NativeMethods.Entity.GetEntity(Id)));

			Physics.OnScriptReload();
            base.OnScriptReloadInternal();
		}

		// TODO: Expose the attachment system properly
#region Attachments
		public Attachment GetAttachment(int index, int characterSlot = 0)
		{
			var ptr = NativeMethods.Entity.GetAttachmentByIndex(this.GetEntityHandle().Handle, index, characterSlot);
			if (ptr == IntPtr.Zero)
				return null;

			return Attachment.TryAdd(ptr);
		}

		public Attachment GetAttachment(string name, int characterSlot = 0)
		{
			var ptr = NativeMethods.Entity.GetAttachmentByName(this.GetEntityHandle().Handle, name, characterSlot);

			return Attachment.TryAdd(ptr);
		}

        public int GetAttachmentCount(int characterSlot = 0)
		{
			return NativeMethods.Entity.GetAttachmentCount(this.GetEntityHandle().Handle, characterSlot); 
		}
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
			return NativeMethods.Entity.AddEntityLink(this.GetEntityHandle().Handle, linkName, otherEntityId, relativeRot, relativePos);
		}

		/// <summary>
		/// Removes an entity link, see Link.
		/// </summary>
		/// <param name="otherEntityId"></param>
		public void Unlink(EntityId otherEntityId)
		{
			NativeMethods.Entity.RemoveEntityLink(this.GetEntityHandle().Handle, otherEntityId);
		}

		/// <summary>
		/// Loads a light source to the specified slot, or to the next available slot.
		/// </summary>
		/// <param name="parameters"></param>
		/// <param name="slot"></param>
		/// <returns>The slot where the light source was loaded, or -1 if loading failed.</returns>
		public int LoadLight(LightParams parameters, int slot = 1)
		{
			return NativeMethods.Entity.LoadLight(this.GetEntityHandle().Handle, slot, parameters);
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
				NativeMethods.Entity.LoadObject(this.GetEntityHandle().Handle, name, slotNumber);
			else if (name.EndsWith("cdf") || name.EndsWith("cga") || name.EndsWith("chr"))
				NativeMethods.Entity.LoadCharacter(this.GetEntityHandle().Handle, name, slotNumber);
			else
				return false;

			return true;
		}

		protected string GetObjectFilePath(int slot = 0)
		{
			return NativeMethods.Entity.GetStaticObjectFilePath(this.GetEntityHandle().Handle, slot);
		}

		public void PlayAnimation(string animationName, AnimationFlags flags = 0, int slot = 0, int layer = 0, float blend = 0.175f, float speed = 1.0f)
		{
			NativeMethods.Entity.PlayAnimation(this.GetEntityHandle().Handle, animationName, slot, layer, blend, speed, flags);
		}

		public void FreeSlot(int slot)
		{
			NativeMethods.Entity.FreeSlot(this.GetEntityHandle().Handle, slot);
		}

		public void AddMovement(ref EntityMovementRequest request)
		{
			NativeMethods.Entity.AddMovement(this.GetAnimatedCharacterHandle().Handle, ref request);
		}

		public Lua.ScriptTable ScriptTable { get { return Lua.ScriptTable.Get(this.GetEntityHandle().Handle); } }

		/// <summary>
		/// Sets / gets the world space entity position.
		/// </summary>
        public Vec3 Position { get { return NativeMethods.Entity.GetWorldPos(this.GetEntityHandle().Handle); } set { NativeMethods.Entity.SetWorldPos(this.GetEntityHandle().Handle, value); } }
		/// <summary>
		/// Sets / gets the world space entity orientation quaternion.
		/// </summary>
        public Quat Rotation { get { return NativeMethods.Entity.GetWorldRotation(this.GetEntityHandle().Handle); } set { NativeMethods.Entity.SetWorldRotation(this.GetEntityHandle().Handle, value); } }

		/// <summary>
		/// Sets / gets the local space entity position.
		/// </summary>
        public Vec3 LocalPosition { get { return NativeMethods.Entity.GetPos(this.GetEntityHandle().Handle); } set { NativeMethods.Entity.SetPos(this.GetEntityHandle().Handle, value); } }
		/// <summary>
		/// Sets / gets the local space entity orientation quaternion.
		/// </summary>
        public Quat LocalRotation { get { return NativeMethods.Entity.GetRotation(this.GetEntityHandle().Handle); } set { NativeMethods.Entity.SetRotation(this.GetEntityHandle().Handle, value); } }

		/// <summary>
		/// Sets / gets the world space entity transformation matrix.
		/// </summary>
        public Matrix34 Transform { get { return NativeMethods.Entity.GetWorldTM(this.GetEntityHandle().Handle); } set { NativeMethods.Entity.SetWorldTM(this.GetEntityHandle().Handle, value); } }
		/// <summary>
		/// Sets / gets the local space entity transformation matrix.
		/// </summary>
        public Matrix34 LocalTransform { get { return NativeMethods.Entity.GetLocalTM(this.GetEntityHandle().Handle); } set { NativeMethods.Entity.SetLocalTM(this.GetEntityHandle().Handle, value); } }

		/// <summary>
		/// Gets the entity axis aligned bounding box in the world space.
		/// </summary>
		public BoundingBox BoundingBox { get { return NativeMethods.Entity.GetWorldBoundingBox(this.GetEntityHandle().Handle); } }
		/// <summary>
		/// Gets the entity axis aligned bounding box in the world space.
		/// </summary>
		public BoundingBox LocalBoundingBox { get { return NativeMethods.Entity.GetBoundingBox(this.GetEntityHandle().Handle); } }

		/// <summary>
		/// Gets / sets the entity name.
		/// </summary>
		public string Name { get { return NativeMethods.Entity.GetName(this.GetEntityHandle().Handle); } set { NativeMethods.Entity.SetName(this.GetEntityHandle().Handle, value); } }
		/// <summary>
		/// Gets / sets the entity flags.
		/// </summary>
		public EntityFlags Flags { get { return NativeMethods.Entity.GetFlags(this.GetEntityHandle().Handle); } set { NativeMethods.Entity.SetFlags(this.GetEntityHandle().Handle, value); } }

		public Material Material { get { return Material.Get(this); } set { Material.Set(this, value); } }

		public EntityId Id { get; set; }

		#region Native handles
		/// <summary>
		/// IEntity handle
		/// </summary>
		internal HandleRef EntityHandleRef { get; set; }
		/// <summary>
		/// IAnimatedCharacter handle
		/// </summary>
		internal HandleRef AnimatedCharacterHandleRef { get; set; }
		#endregion
	}
}