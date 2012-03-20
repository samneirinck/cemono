using System.Runtime.CompilerServices;
using System;

namespace CryEngine
{
	public partial class Entity
	{
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _Physicalize(uint entityId, PhysicalizationParams physicalizationParams);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _Sleep(uint entityId, bool sleep);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _BreakIntoPieces(uint entityId, int slot, int piecesSlot, BreakageParameters breakageParams);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _AddImpulse(uint entityId, ActionImpulse actionImpulse);

		internal void InitPhysics()
		{
			_physics = new PhysicsParams(Id);
			_physics.Slot = 0;
		}

		public Vec3 Velocity
		{
			get { return _GetVelocity(Id); }

			set { _SetVelocity(Id, value); }
		}

		PhysicsParams _physics;
		public PhysicsParams Physics { get { return _physics; } set { _physics = value; _physics._entityId = Id; } }
	}

	/// <summary>
	/// Wrapper class to make physics parameters more intuitive.
	/// </summary>
	public class PhysicsParams
	{
		//These are the params that are actually sent to the engine
		internal PhysicalizationParams _params;
		internal EntityId _entityId;

		public PhysicsParams() { }

		internal PhysicsParams(EntityId id)
		{
			_entityId = id;
			AutoUpdate = true;
			
			_params = new PhysicalizationParams();
		}

		public void Break(BreakageParameters breakageParams)
		{
			Entity._BreakIntoPieces(_entityId, 0, 0, breakageParams);
		}

		#region Basics
		/// <summary>
		/// If true, physics value updates will be automatically applied. Otherwise, Save() must be called manually.
		/// </summary>
		public bool AutoUpdate { get; set; }

		internal bool resting;
		/// <summary>
		/// Determines if this physical entity is in a sleeping state or not. (Will not be affected by gravity)
		/// Autoamtically wakes upon collision.
		/// </summary>
		public bool Resting { get { return resting; } set { resting = value; Entity._Sleep(_entityId, value); } }

		/// <summary>
		/// Save the current physics settings.
		/// </summary>
		public void Save()
		{
			Entity._Physicalize(_entityId, _params);
		}

		/// <summary>
		/// Clears the current physics settings.
		/// </summary>
		public void Clear()
		{
			_params = new PhysicalizationParams();
		}

		public void AddImpulse(Vec3 impulse, Vec3 angImpulse = default(Vec3), Vec3? point = null)
		{
			var actionImpulse = new ActionImpulse();

			actionImpulse.impulse = impulse;
			actionImpulse.angImpulse = angImpulse;
			actionImpulse.point = point ?? Entity.GetEntity(_entityId).Position;

			Entity._AddImpulse(_entityId, actionImpulse);
		}

		/// <summary>
		/// The mass of the entity in kg.
		/// </summary>
		public float Mass
		{
			get { return _params.Mass; }
			set { _params.Mass = value; if(AutoUpdate) Save(); }
		}

		public float Density
		{
			get { return _params.Density; }
			set { _params.Density = value; if(AutoUpdate) Save(); }
		}

		/// <summary>
		/// The entity slot for which these physical parameters apply.
		/// </summary>
		public int Slot
		{
			get { return _params.Slot; }
			set { _params.Slot = value; if(AutoUpdate) Save(); }
		}

		public PhysicalizationType Type
		{
			get { return _params.PhysicalizationType; }
            set { _params.PhysicalizationType = value; if (AutoUpdate) Save(); }
		}
		#endregion

		#region Characters
		/// <summary>
		/// For characters: the scale of force in joint springs.
		/// </summary>
		public float Stiffness
		{
			get { return _params.StiffnessScale; }
			set { _params.StiffnessScale = value; if(AutoUpdate) Save(); }
		}

		#endregion
	}

	internal struct ActionImpulse
	{
		public Vec3 impulse;
		public Vec3 angImpulse;	// optional
		public Vec3 point; // point of application, in world CS, optional 
		public int partid;	// receiver part identifier
		public int ipart; // alternatively, part index can be used
		public int iApplyTime; // 0-apply immediately, 1-apply before the next time step, 2-apply after the next time step
		public int iSource; // reserved for internal use
	}

	public enum PhysicsApplyTime
	{
		Immediate = 0,
		PreStep = 1,
		PostStep = 2
	}

	public enum BreakageType
	{
		Destroy = 0,
		Freeze_Shatter
	}

	public struct BreakageParameters
	{
		public BreakageType type;					// Type of the breakage.
		public float fParticleLifeTime;		// Average lifetime of particle pieces.
		public int nGenericCount;				// If not 0, force particle pieces to spawn generically, this many times.
		public bool bForceEntity;					// Force pieces to spawn as entities.
		public bool bMaterialEffects;			// Automatically create "destroy" and "breakage" material effects on pieces.
		public bool bOnlyHelperPieces;		// Only spawn helper pieces.

		// Impulse params.
		public float fExplodeImpulse;			// Outward impulse to apply.
		public Vec3 vHitImpulse;					// Hit impulse and center to apply.
		public Vec3 vHitPoint;
	}

	public struct PhysicalizationParams
	{
		public PhysicalizationType PhysicalizationType { get; set; }

		/// <summary>
		/// Index of object slot, -1 if all slots should be used.
		/// </summary>
        public int Slot { get; set; }

		/// <summary>
		/// Only one either density or mass must be set, parameter set to 0 is ignored.
		/// </summary>
        public float Density { get; set; }
        public float Mass { get; set; }

		/// <summary>
		/// Used for character physicalization (Scale of force in character joint's springs).
		/// </summary>
        public float StiffnessScale { get; set; }

		public PlayerDimensions playerDimensions;
		public PlayerDynamics playerDynamics;
	}

	public struct PlayerDynamics
	{
		/// <summary>
		/// inertia koefficient, the more it is, the less inertia is; 0 means no inertia
		/// </summary>
        public float Inertia { get; set; }
		/// <summary>
		/// inertia on acceleration koefficient
		/// </summary>
        public float InertiaAcceleration { get; set; }
		/// <summary>
		/// air control koefficient 0..1, 1 - special value (total control of movement)
		/// </summary>
        public float AirControl { get; set; }
		/// <summary>
        /// standard air resistance koefficient
		/// </summary>
        public float AirResistance { get; set; }
		/// <summary>
		/// gravity vector
		/// </summary>
		public Vec3 Gravity { get; set; }
		/// <summary>
		/// vertical camera shake speed after landings
		/// </summary>
		public float NodSpeed { get; set; }
		/// <summary>
		/// whether entity is swimming (is not bound to ground plane)
		/// </summary>
		public bool Swimming { get; set; }
		/// <summary>
		/// mass (in kg)
		/// </summary>
		public float Mass { get; set; }
		/// <summary>
		/// surface identifier for collisions
		/// </summary>
		public int SurfaceId { get; set; }
		/// <summary>
		/// if surface slope is more than this angle, player starts sliding (angle is in radians)
		/// </summary>
		public float MinSlideAngle { get; set; }
		/// <summary>
		/// player cannot climb surface which slope is steeper than this angle
		/// </summary>
		public float MaxClimbAngle { get; set; }
		/// <summary>
		/// player is not allowed to jump towards ground if this angle is exceeded
		/// </summary>
		public float MaxJumpAngle { get; set; }
		/// <summary>
		/// player starts falling when slope is steeper than this
		/// </summary>
		public float MinFallAngle { get; set; }
		/// <summary>
		/// player cannot stand of surfaces that are moving faster than this
		/// </summary>
		public float MaxVelGround { get; set; }
		/// <summary>
		/// forcefully turns on inertia for that duration after receiving an impulse
		/// </summary>
		public float TimeImpulseRecover { get; set; }
		/// <summary>
		/// entity types to check collisions against
		/// </summary>
		public int CollisionTypes { get; set; }
		/// <summary>
		/// ignore collisions with this *living entity* (doesn't work with other entity types)
		/// </summary>
		public EntityId LivingEntToIgnore { get; set; }
		/// <summary>
		/// 0 disables all simulation for the character, apart from moving along the requested velocity
		/// </summary>
		public bool Active { get; set; }
		/// <summary>
		/// requests that the player rolls back to that time and re-exucutes pending actions during the next step
		/// </summary>
		public int RequestedTime { get; set; }
	}

	public struct PlayerDimensions
	{
		/// <summary>
		/// offset from central ground position that is considered entity center
		/// </summary>
		public float HeightPivot { get; set; }
		/// <summary>
		/// vertical offset of camera
		/// </summary>
		public float HeightEye { get; set; }
		/// <summary>
		/// collision cylinder dimensions
		/// </summary>
		public Vec3 SizeCollider { get; set; }
		/// <summary>
		/// vertical offset of collision geometry center
		/// </summary>
		public float HeightCollider { get; set; }
		/// <summary>
		/// radius of the 'head' geometry (used for camera offset)
		/// </summary>
		public float HeadRadius { get; set; }
		/// <summary>
		/// center.z of the head geometry
		/// </summary>
		public float HeightHead { get; set; }
		/// <summary>
		/// unprojection direction to test in case the new position overlaps with the environment (can be 0 for 'auto')
		/// </summary>
		public Vec3 UnprojectionDirection { get; set; }
		/// <summary>
		/// maximum allowed unprojection
		/// </summary>
		public float MaxUnprojection { get; set; }
		/// <summary>
		/// switches between capsule and cylinder collider geometry
		/// </summary>
		public bool UseCapsule { get; set; }
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
