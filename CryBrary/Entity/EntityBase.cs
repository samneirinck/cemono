using System;
using System.Runtime.CompilerServices;

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
            return NativeMethods.Entity.GetSlotFlags(EntityPointer, slot);
		}

		public void SetSlotFlags(EntitySlotFlags flags, int slot = 0)
		{
            NativeMethods.Entity.SetSlotFlags(EntityPointer, slot, flags);
		}

		public Vec3 Velocity
		{
			get { return NativeMethods.Physics.GetVelocity(EntityPointer); }

            set { NativeMethods.Physics.SetVelocity(EntityPointer, value); }
		}

		EntityPhysics _physics;
		public EntityPhysics Physics
		{
			get { return _physics ?? (_physics = new EntityPhysics(this)); }
		}

        internal override void OnScriptReloadInternal()
		{
            EntityPointer = NativeMethods.Entity.GetEntity(Id);

			Physics.OnScriptReload();
            base.OnScriptReloadInternal();
		}

		// TODO: Expose the attachment system properly
#region Attachments
		public Material GetAttachmentMaterial(int index)
		{
            var ptr = NativeMethods.Entity.GetAttachmentMaterialByIndex(EntityPointer, index);

			return Material.TryAdd(ptr);
		}

		public Material GetAttachmentMaterial(string attachmentName)
		{
            var ptr = NativeMethods.Entity.GetAttachmentMaterial(EntityPointer, attachmentName);

			return Material.TryAdd(ptr);
		}

		public void SetAttachmentMaterial(int index, Material newMaterial)
		{
			NativeMethods.Entity.SetAttachmentMaterialByIndex(EntityPointer, index, newMaterial.HandleRef.Handle);
		}

		public void SetAttachmentMaterial(string name, Material newMaterial)
		{
			NativeMethods.Entity.SetAttachmentMaterial(EntityPointer, name, newMaterial.HandleRef.Handle);
		}

        public int AttachmentCount { get { return NativeMethods.Entity.GetAttachmentCount(EntityPointer); } }
#endregion

		/// <summary>
		/// Sets / gets the world space entity position.
		/// </summary>
        public Vec3 Position { get { return NativeMethods.Entity.GetWorldPos(EntityPointer); } set { NativeMethods.Entity.SetWorldPos(EntityPointer, value); } }
		/// <summary>
		/// Sets / gets the world space entity orientation quaternion.
		/// </summary>
        public Quat Rotation { get { return NativeMethods.Entity.GetWorldRotation(EntityPointer); } set { NativeMethods.Entity.SetWorldRotation(EntityPointer, value); } }

		/// <summary>
		/// Sets / gets the local space entity position.
		/// </summary>
        public Vec3 LocalPosition { get { return NativeMethods.Entity.GetPos(EntityPointer); } set { NativeMethods.Entity.SetPos(EntityPointer, value); } }
		/// <summary>
		/// Sets / gets the local space entity orientation quaternion.
		/// </summary>
        public Quat LocalRotation { get { return NativeMethods.Entity.GetRotation(EntityPointer); } set { NativeMethods.Entity.SetRotation(EntityPointer, value); } }

		/// <summary>
		/// Sets / gets the world space entity transformation matrix.
		/// </summary>
        public Matrix34 Transform { get { return NativeMethods.Entity.GetWorldTM(EntityPointer); } set { NativeMethods.Entity.SetWorldTM(EntityPointer, value); } }
		/// <summary>
		/// Sets / gets the local space entity transformation matrix.
		/// </summary>
        public Matrix34 LocalTransform { get { return NativeMethods.Entity.GetLocalTM(EntityPointer); } set { NativeMethods.Entity.SetLocalTM(EntityPointer, value); } }

		/// <summary>
		/// Gets the entity axis aligned bounding box in the world space.
		/// </summary>
        public BoundingBox BoundingBox { get { return NativeMethods.Entity.GetWorldBoundingBox(EntityPointer); } }
		/// <summary>
		/// Gets the entity axis aligned bounding box in the world space.
		/// </summary>
        public BoundingBox LocalBoundingBox { get { return NativeMethods.Entity.GetBoundingBox(EntityPointer); } }

		/// <summary>
		/// Gets / sets the entity name.
		/// </summary>
        public string Name { get { return NativeMethods.Entity.GetName(EntityPointer); } set { NativeMethods.Entity.SetName(EntityPointer, value); } }
		/// <summary>
		/// Gets / sets the entity flags.
		/// </summary>
        public EntityFlags Flags { get { return NativeMethods.Entity.GetFlags(EntityPointer); } set { NativeMethods.Entity.SetFlags(EntityPointer, value); } }

		public Material Material { get { return Material.Get(this); } set { Material.Set(this, value); } }

		internal IntPtr EntityPointer { get; set; }
		public EntityId Id { get; set; }
	}
}