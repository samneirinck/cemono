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
		private static INativeEntityMethods _nativeEntityMethods;
		internal static INativeEntityMethods NativeEntityMethods
		{
			get { return _nativeEntityMethods ?? (_nativeEntityMethods = new NativeEntityMethods()); }
			set { _nativeEntityMethods = value; }
		}

        private static INativePhysicsMethods _nativePhysicsMethods;
        internal static INativePhysicsMethods NativePhysicsMethods
        {
            get { return _nativePhysicsMethods ?? (_nativePhysicsMethods = new NativePhysicsMethods()); }
            set { _nativePhysicsMethods = value; }
        }

		#region Externals
		#endregion

		#region Callbacks
		/// <summary>
		/// This callback is called when this entity has finished spawning. The entity has been created and added to the list of entities.
		/// </summary>
		public virtual void OnSpawn() { }
		#endregion

		public EntitySlotFlags GetSlotFlags(int slot = 0)
		{
            return NativeEntityMethods.GetSlotFlags(EntityPointer, slot);
		}

		public void SetSlotFlags(EntitySlotFlags flags, int slot = 0)
		{
            NativeEntityMethods.SetSlotFlags(EntityPointer, slot, flags);
		}

		public Vec3 Velocity
		{
			get { return NativePhysicsMethods.GetVelocity(EntityPointer); }

            set { NativePhysicsMethods.SetVelocity(EntityPointer, value); }
		}

		EntityPhysics _physics;
		public EntityPhysics Physics
		{
			get { return _physics ?? (_physics = new EntityPhysics(this)); }
		}

        internal override void OnScriptReloadInternal()
		{
            EntityPointer = NativeEntityMethods.GetEntity(Id);

			Physics.OnScriptReload();
            base.OnScriptReloadInternal();
		}

		// TODO: Expose the attachment system properly
#region Attachments
		public Material GetAttachmentMaterial(int index)
		{
            var ptr = NativeEntityMethods.GetAttachmentMaterialByIndex(EntityPointer, index);

			return Material.TryAdd(ptr);
		}

		public Material GetAttachmentMaterial(string attachmentName)
		{
            var ptr = NativeEntityMethods.GetAttachmentMaterial(EntityPointer, attachmentName);

			return Material.TryAdd(ptr);
		}

		public void SetAttachmentMaterial(int index, Material newMaterial)
		{
            NativeEntityMethods.SetAttachmentMaterialByIndex(EntityPointer, index, newMaterial.MaterialPointer);
		}

		public void SetAttachmentMaterial(string name, Material newMaterial)
		{
            NativeEntityMethods.SetAttachmentMaterial(EntityPointer, name, newMaterial.MaterialPointer);
		}

        public int AttachmentCount { get { return NativeEntityMethods.GetAttachmentCount(EntityPointer); } }
#endregion

		/// <summary>
		/// Sets / gets the world space entity position.
		/// </summary>
        public Vec3 Position { get { return NativeEntityMethods.GetWorldPos(EntityPointer); } set { NativeEntityMethods.SetWorldPos(EntityPointer, value); } }
		/// <summary>
		/// Sets / gets the world space entity orientation quaternion.
		/// </summary>
        public Quat Rotation { get { return NativeEntityMethods.GetWorldRotation(EntityPointer); } set { NativeEntityMethods.SetWorldRotation(EntityPointer, value); } }

		/// <summary>
		/// Sets / gets the local space entity position.
		/// </summary>
        public Vec3 LocalPosition { get { return NativeEntityMethods.GetPos(EntityPointer); } set { NativeEntityMethods.SetPos(EntityPointer, value); } }
		/// <summary>
		/// Sets / gets the local space entity orientation quaternion.
		/// </summary>
        public Quat LocalRotation { get { return NativeEntityMethods.GetRotation(EntityPointer); } set { NativeEntityMethods.SetRotation(EntityPointer, value); } }

		/// <summary>
		/// Sets / gets the world space entity transformation matrix.
		/// </summary>
        public Matrix34 Transform { get { return NativeEntityMethods.GetWorldTM(EntityPointer); } set { NativeEntityMethods.SetWorldTM(EntityPointer, value); } }
		/// <summary>
		/// Sets / gets the local space entity transformation matrix.
		/// </summary>
        public Matrix34 LocalTransform { get { return NativeEntityMethods.GetLocalTM(EntityPointer); } set { NativeEntityMethods.SetLocalTM(EntityPointer, value); } }

		/// <summary>
		/// Gets the entity axis aligned bounding box in the world space.
		/// </summary>
        public BoundingBox BoundingBox { get { return NativeEntityMethods.GetWorldBoundingBox(EntityPointer); } }
		/// <summary>
		/// Gets the entity axis aligned bounding box in the world space.
		/// </summary>
        public BoundingBox LocalBoundingBox { get { return NativeEntityMethods.GetBoundingBox(EntityPointer); } }

		/// <summary>
		/// Gets / sets the entity name.
		/// </summary>
        public string Name { get { return NativeEntityMethods.GetName(EntityPointer); } set { NativeEntityMethods.SetName(EntityPointer, value); } }
		/// <summary>
		/// Gets / sets the entity flags.
		/// </summary>
        public EntityFlags Flags { get { return NativeEntityMethods.GetFlags(EntityPointer); } set { NativeEntityMethods.SetFlags(EntityPointer, value); } }

		public Material Material { get { return Material.Get(this); } set { Material.Set(this, value); } }

		internal IntPtr EntityPointer { get; set; }
		public EntityId Id { get; set; }
	}
}