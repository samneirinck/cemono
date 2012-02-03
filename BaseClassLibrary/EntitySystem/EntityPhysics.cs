namespace CryEngine
{
	/// <summary>
	/// Wrapper class to make physics parameters more intuitive.
	/// </summary>
	public class PhysicsParams
	{
		//These are the params that are actually sent to the engine
		internal PhysicalizationParams _params = new PhysicalizationParams();
		internal StaticEntity _entity;

		internal PhysicsParams(uint id)
		{
			_entity = EntitySystem.GetEntity(id);
			AutoUpdate = true;
		}

		#region Basics
		/// <summary>
		/// If true, physics value updates will be automatically applied. Otherwise, Save() must be called manually.
		/// </summary>
		public bool AutoUpdate { get; set; }

		/// <summary>
		/// Save the current physics settings.
		/// </summary>
		public void Save()
		{
			StaticEntity._Physicalize(_entity.Id, _params);
		}

		/// <summary>
		/// Clears the current physics settings.
		/// </summary>
		public void Clear()
		{
			_params = new PhysicalizationParams();
		}

		/// <summary>
		/// The mass of the entity in kg.
		/// </summary>
		public float Mass
		{
			get { return _params.mass; }
			set { _params.mass = value; _params.density = -1; if(AutoUpdate) Save(); }
		}

		public float Density
		{
			get { return _params.density; }
			set { _params.density = value; _params.mass = -1; if(AutoUpdate) Save(); }
		}

		/// <summary>
		/// The entity slot for which these physical parameters apply.
		/// </summary>
		public int Slot
		{
			get { return _params.slot; }
			set { _params.slot = value; if(AutoUpdate) Save(); }
		}

		public PhysicalizationType Type
		{
			get { return _params.type; }
			set { _params.type = value; if(AutoUpdate) Save(); }
		}
		#endregion

		#region Characters
		/// <summary>
		/// For characters: the scale of force in joint springs.
		/// </summary>
		public float Stiffness
		{
			get { return _params.stiffnessScale; }
			set { _params.stiffnessScale = value; if(AutoUpdate) Save(); }
		}

		#endregion
	}

	internal struct PhysicalizationParams
	{
		public PhysicalizationType type;

		/// <summary>
		/// Index of object slot, -1 if all slots should be used.
		/// </summary>
		public int slot;

		/// <summary>
		/// Only one either density or mass must be set, parameter set to 0 is ignored.
		/// </summary>
		public float density;
		public float mass;

		/// <summary>
		/// Used for character physicalization (Scale of force in character joint's springs).
		/// </summary>
		public float stiffnessScale;

		public PlayerDimensions playerDimensions;
		public PlayerDynamics playerDynamics;

		public struct PlayerDynamics
		{
			/// <summary>
			/// inertia koefficient, the more it is, the less inertia is; 0 means no inertia
			/// </summary>
			public float kInertia;
			/// <summary>
			/// inertia on acceleration
			/// </summary>
			public float kInertiaAccel;
			/// <summary>
			/// air control koefficient 0..1, 1 - special value (total control of movement)
			/// </summary>
			public float kAirControl;
			/// <summary>
			/// standard air resistance 
			/// </summary>
			public float kAirResistance;
			/// <summary>
			/// gravity vector, utilizes sv_gravity if null.
			/// </summary>
			public Vec3 gravity;
			/// <summary>
			/// vertical camera shake speed after landings
			/// </summary>
			public float nodSpeed;
			/// <summary>
			/// whether entity is swimming (is not bound to ground plane)
			/// </summary>
			public bool swimming;
			/// <summary>
			/// mass (in kg)
			/// </summary>
			public float mass;
			/// <summary>
			/// surface identifier for collisions
			/// </summary>
			public int surface_idx;
			/// <summary>
			/// if surface slope is more than this angle, player starts sliding (angle is in radians)
			/// </summary>
			public float minSlideAngle;
			/// <summary>
			/// player cannot climb surface which slope is steeper than this angle
			/// </summary>
			public float maxClimbAngle;
			/// <summary>
			/// player is not allowed to jump towards ground if this angle is exceeded
			/// </summary>
			public float maxJumpAngle;
			/// <summary>
			/// player starts falling when slope is steeper than this
			/// </summary>
			public float minFallAngle;
			/// <summary>
			/// player cannot stand of surfaces that are moving faster than this
			/// </summary>
			public float maxVelGround;
			/// <summary>
			/// forcefully turns on inertia for that duration after receiving an impulse
			/// </summary>
			public float timeImpulseRecover;
			/// <summary>
			/// entity types to check collisions against
			/// </summary>
			public int collTypes;
			/// <summary>
			/// ignore collisions with this *living entity* (doesn't work with other entity types)
			/// </summary>
			public uint livingEntToIgnore;
			/// <summary>
			/// 0 disables all simulation for the character, apart from moving along the requested velocity
			/// </summary>
			public bool active;
			/// <summary>
			/// requests that the player rolls back to that time and re-exucutes pending actions during the next step
			/// </summary>
			public int iRequestedTime;
		}

		public struct PlayerDimensions
		{
			/// <summary>
			/// offset from central ground position that is considered entity center
			/// </summary>
			public float heightPivot;
			/// <summary>
			/// vertical offset of camera
			/// </summary>
			public float heightEye;
			/// <summary>
			/// collision cylinder dimensions
			/// </summary>
			public Vec3 sizeCollider;
			/// <summary>
			/// vertical offset of collision geometry center
			/// </summary>
			public float heightCollider;
			/// <summary>
			/// radius of the 'head' geometry (used for camera offset)
			/// </summary>
			public float headRadius;
			/// <summary>
			/// center.z of the head geometry
			/// </summary>
			public float heightHead;
			/// <summary>
			/// unprojection direction to test in case the new position overlaps with the environment (can be 0 for 'auto')
			/// </summary>
			public Vec3 dirUnproj;
			/// <summary>
			/// maximum allowed unprojection
			/// </summary>
			public float maxUnproj;
			/// <summary>
			/// switches between capsule and cylinder collider geometry
			/// </summary>
			public bool useCapsule;
		}


	}

	public enum PhysicalizationType
	{
		None = 0,
		Static,
		Rigid,
		WheeledVehicle,
		Living,
		Particle,
		Articulated,
		Rope,
		Soft,
		Area
	}
}
