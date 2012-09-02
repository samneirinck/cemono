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
			return NativeMethods.Entity.GetSlotFlags(HandleRef.Handle, slot);
		}

		public void SetSlotFlags(EntitySlotFlags flags, int slot = 0)
		{
			NativeMethods.Entity.SetSlotFlags(HandleRef.Handle, slot, flags);
		}

		public Vec3 Velocity
		{
			get { return NativeMethods.Physics.GetVelocity(HandleRef.Handle); }

			set { NativeMethods.Physics.SetVelocity(HandleRef.Handle, value); }
		}

		EntityPhysics _physics;
		public EntityPhysics Physics
		{
			get { return _physics ?? (_physics = new EntityPhysics(this)); }
		}

        internal override void OnScriptReloadInternal()
		{
            HandleRef = new HandleRef(this, NativeMethods.Entity.GetEntity(Id));

			Physics.OnScriptReload();
            base.OnScriptReloadInternal();
		}

		// TODO: Expose the attachment system properly
#region Attachments
		public Material GetAttachmentMaterial(int index)
		{
			var ptr = NativeMethods.Entity.GetAttachmentMaterialByIndex(HandleRef.Handle, index);

			return Material.TryAdd(ptr);
		}

		public Material GetAttachmentMaterial(string attachmentName)
		{
			var ptr = NativeMethods.Entity.GetAttachmentMaterial(HandleRef.Handle, attachmentName);

			return Material.TryAdd(ptr);
		}

		public void SetAttachmentMaterial(int index, Material newMaterial)
		{
			NativeMethods.Entity.SetAttachmentMaterialByIndex(HandleRef.Handle, index, newMaterial.HandleRef.Handle);
		}

		public void SetAttachmentMaterial(string name, Material newMaterial)
		{
			NativeMethods.Entity.SetAttachmentMaterial(HandleRef.Handle, name, newMaterial.HandleRef.Handle);
		}

        public int AttachmentCount { get { return NativeMethods.Entity.GetAttachmentCount(HandleRef.Handle); } }
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
			return NativeMethods.Entity.AddEntityLink(linkName, otherEntityId, relativeRot, relativePos);
		}

		/// <summary>
		/// Removes an entity link, see Link.
		/// </summary>
		/// <param name="otherEntityId"></param>
		public void Unlink(EntityId otherEntityId)
		{
			NativeMethods.Entity.RemoveEntityLink(otherEntityId);
		}

		public Lua.ScriptTable ScriptTable { get { return Lua.ScriptTable.Get(HandleRef.Handle); } }

		/// <summary>
		/// Sets / gets the world space entity position.
		/// </summary>
        public Vec3 Position { get { return NativeMethods.Entity.GetWorldPos(HandleRef.Handle); } set { NativeMethods.Entity.SetWorldPos(HandleRef.Handle, value); } }
		/// <summary>
		/// Sets / gets the world space entity orientation quaternion.
		/// </summary>
        public Quat Rotation { get { return NativeMethods.Entity.GetWorldRotation(HandleRef.Handle); } set { NativeMethods.Entity.SetWorldRotation(HandleRef.Handle, value); } }

		/// <summary>
		/// Sets / gets the local space entity position.
		/// </summary>
        public Vec3 LocalPosition { get { return NativeMethods.Entity.GetPos(HandleRef.Handle); } set { NativeMethods.Entity.SetPos(HandleRef.Handle, value); } }
		/// <summary>
		/// Sets / gets the local space entity orientation quaternion.
		/// </summary>
        public Quat LocalRotation { get { return NativeMethods.Entity.GetRotation(HandleRef.Handle); } set { NativeMethods.Entity.SetRotation(HandleRef.Handle, value); } }

		/// <summary>
		/// Sets / gets the world space entity transformation matrix.
		/// </summary>
        public Matrix34 Transform { get { return NativeMethods.Entity.GetWorldTM(HandleRef.Handle); } set { NativeMethods.Entity.SetWorldTM(HandleRef.Handle, value); } }
		/// <summary>
		/// Sets / gets the local space entity transformation matrix.
		/// </summary>
        public Matrix34 LocalTransform { get { return NativeMethods.Entity.GetLocalTM(HandleRef.Handle); } set { NativeMethods.Entity.SetLocalTM(HandleRef.Handle, value); } }

		/// <summary>
		/// Gets the entity axis aligned bounding box in the world space.
		/// </summary>
		public BoundingBox BoundingBox { get { return NativeMethods.Entity.GetWorldBoundingBox(HandleRef.Handle); } }
		/// <summary>
		/// Gets the entity axis aligned bounding box in the world space.
		/// </summary>
		public BoundingBox LocalBoundingBox { get { return NativeMethods.Entity.GetBoundingBox(HandleRef.Handle); } }

		/// <summary>
		/// Gets / sets the entity name.
		/// </summary>
		public string Name { get { return NativeMethods.Entity.GetName(HandleRef.Handle); } set { NativeMethods.Entity.SetName(HandleRef.Handle, value); } }
		/// <summary>
		/// Gets / sets the entity flags.
		/// </summary>
		public EntityFlags Flags { get { return NativeMethods.Entity.GetFlags(HandleRef.Handle); } set { NativeMethods.Entity.SetFlags(HandleRef.Handle, value); } }

		public Material Material { get { return Material.Get(this); } set { Material.Set(this, value); } }

		public HandleRef HandleRef { get; set; }
		public EntityId Id { get; set; }
	}
}