using System;
using System.Runtime.CompilerServices;

namespace CryEngine
{
	internal interface INativeEntityMethods
	{
		bool RegisterClass(EntityConfig config);
	}

	/// <summary>
	/// Contains common entity functionality.
	/// </summary>
	public abstract class EntityBase : CryScriptInstance
	{
		private static INativeEntityMethods _methods;
		internal static INativeEntityMethods Methods
		{
			get { return _methods ?? (_methods = new EntityMethods()); }
			set { _methods = value; }
		}

		class EntityMethods : INativeEntityMethods
		{
			/// <summary>
			/// Register a new entity type.
			/// </summary>
			/// <param name="config">The Entity configuration.</param>
			/// <returns>True if registration succeeded, otherwise false.</returns>
			public bool RegisterClass(EntityConfig config)
			{
				return EntityBase._RegisterEntityClass(config.registerParams, config.properties);
			}
		}

		#region Externals
		/// <summary>
		/// Spawns an entity with the specified parameters.
		/// </summary>
		/// <param name="spawnParams">The <see cref="EntitySpawnParams"/></param>
		/// <param name="autoInit">Should the entity automatically be initialized?</param>
		/// <returns></returns>
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static EntityInfo _SpawnEntity(EntitySpawnParams spawnParams, bool autoInit = true);
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
		extern internal static bool _RegisterEntityClass(EntityRegisterParams registerParams, object[] properties);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _Physicalize(IntPtr ptr, PhysicalizationParams physicalizationParams);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _Sleep(IntPtr ptr, bool sleep);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _BreakIntoPieces(IntPtr ptr, int slot, int piecesSlot, BreakageParameters breakageParams);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _AddImpulse(IntPtr ptr, ActionImpulse actionImpulse);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static Vec3 _GetVelocity(IntPtr ptr);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _SetVelocity(IntPtr ptr, Vec3 velocity);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static string _GetName(IntPtr ptr);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _SetName(IntPtr ptr, string name);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static EntityFlags _GetFlags(IntPtr ptr);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _SetFlags(IntPtr ptr, EntityFlags name);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _AddMovement(IntPtr ptr, ref EntityMovementRequest request);

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
		extern internal static string _GetPropertyValue(IntPtr ptr, string propertyName);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _LoadObject(IntPtr ptr, string fileName, int slot);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static string _GetStaticObjectFilePath(IntPtr ptr, int slot);

		/// <summary>
		/// Loads an non-static model on the object (.chr, .cdf, .cga)
		/// </summary>
		/// <param name="ptr"></param>
		/// <param name="fileName"></param>
		/// <param name="slot"></param>
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _LoadCharacter(IntPtr ptr, string fileName, int slot);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _CreateGameObjectForEntity(IntPtr ptr);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static int _GetAttachmentCount(IntPtr entPtr);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static IntPtr _GetAttachmentMaterialByIndex(IntPtr entPtr, int index);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _SetAttachmentMaterialByIndex(IntPtr entPtr, int index, IntPtr materialPtr);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static IntPtr _GetAttachmentMaterial(IntPtr entPtr, string name);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static int _SetAttachmentMaterial(IntPtr entPtr, string name, IntPtr materialPtr);
		#endregion

		#region Callbacks
		/// <summary>
		/// This callback is called when this entity has finished spawning. The entity has been created and added to the list of entities.
		/// </summary>
		public virtual void OnSpawn() { }
		#endregion

		internal void InitPhysics()
		{
			_physics = new PhysicsParams(this);
			_physics.Slot = 0;
		}

		public EntitySlotFlags GetSlotFlags(int slot = 0)
		{
			return _GetSlotFlags(EntityPointer, slot);
		}

		public void SetSlotFlags(EntitySlotFlags flags, int slot = 0)
		{
			_SetSlotFlags(EntityPointer, slot, flags);
		}

		public Vec3 Velocity
		{
			get { return _GetVelocity(EntityPointer); }

			set { _SetVelocity(EntityPointer, value); }
		}

		PhysicsParams _physics;
		public PhysicsParams Physics
		{
			get { return _physics; }
			set
			{
				_physics = value;

				_physics._entity = this;
			}
		}

		public override void OnScriptReload()
		{
			EntityPointer = _GetEntity(Id);
		}

		// TODO: Expose the attachment system properly
#region Attachments
		public Material GetAttachmentMaterial(int index)
		{
			var ptr = _GetAttachmentMaterialByIndex(EntityPointer, index);

			return Material.TryAdd(ptr);
		}

		public Material GetAttachmentMaterial(string attachmentName)
		{
			var ptr = _GetAttachmentMaterial(EntityPointer, attachmentName);

			return Material.TryAdd(ptr);
		}

		public void SetAttachmentMaterial(int index, Material newMaterial)
		{
			_SetAttachmentMaterialByIndex(EntityPointer, index, newMaterial.MaterialPointer);
		}

		public void SetAttachmentMaterial(string name, Material newMaterial)
		{
			_SetAttachmentMaterial(EntityPointer, name, newMaterial.MaterialPointer);
		}

		public int AttachmentCount { get { return _GetAttachmentCount(EntityPointer); } }
#endregion

		/// <summary>
		/// Sets / gets the world space entity position.
		/// </summary>
		public Vec3 Position { get { return _GetWorldPos(EntityPointer); } set { _SetWorldPos(EntityPointer, value); } }
		/// <summary>
		/// Sets / gets the world space entity orientation quaternion.
		/// </summary>
		public Quat Rotation { get { return _GetWorldRotation(EntityPointer); } set { _SetWorldRotation(EntityPointer, value); } }

		/// <summary>
		/// Sets / gets the local space entity position.
		/// </summary>
		public Vec3 LocalPosition { get { return _GetPos(EntityPointer); } set { _SetPos(EntityPointer, value); } }
		/// <summary>
		/// Sets / gets the local space entity orientation quaternion.
		/// </summary>
		public Quat LocalRotation { get { return _GetRotation(EntityPointer); } set { _SetRotation(EntityPointer, value); } }

		/// <summary>
		/// Sets / gets the world space entity transformation matrix.
		/// </summary>
		public Matrix34 Transform { get { return _GetWorldTM(EntityPointer); } set { _SetWorldTM(EntityPointer, value); } }
		/// <summary>
		/// Sets / gets the local space entity transformation matrix.
		/// </summary>
		public Matrix34 LocalTransform { get { return _GetLocalTM(EntityPointer); } set { _SetLocalTM(EntityPointer, value); } }

		/// <summary>
		/// Gets the entity axis aligned bounding box in the world space.
		/// </summary>
		public BoundingBox BoundingBox { get { return _GetWorldBoundingBox(EntityPointer); } }
		/// <summary>
		/// Gets the entity axis aligned bounding box in the world space.
		/// </summary>
		public BoundingBox LocalBoundingBox { get { return _GetBoundingBox(EntityPointer); } }

		/// <summary>
		/// Gets / sets the entity name.
		/// </summary>
		public string Name { get { return _GetName(EntityPointer); } set { _SetName(EntityPointer, value); } }
		/// <summary>
		/// Gets / sets the entity flags.
		/// </summary>
		public EntityFlags Flags { get { return _GetFlags(EntityPointer); } set { _SetFlags(EntityPointer, value); } }

		public Material Material { get { return Material.Get(this); } set { Material.Set(this, value); } }

		internal IntPtr EntityPointer { get; set; }
		public EntityId Id { get; set; }
	}
}
