using System;
using CryEngine.Native;

using System.Runtime.InteropServices;

namespace CryEngine
{
	/// <summary>
	/// Wrapper class to make physics parameters more intuitive.
	/// </summary>
	public class EntityPhysics
	{
		internal EntityPhysics() { Clear(); }

		internal EntityPhysics(EntityBase _entity)
		{
			entity = _entity;

			Clear();

			PhysicsPointer = NativeMethods.Physics.GetPhysicalEntity(entity.GetEntityHandle().Handle);

			AutoUpdate = true;
		}

		internal void OnScriptReload()
		{
			PhysicsPointer = NativeMethods.Physics.GetPhysicalEntity(entity.GetEntityHandle().Handle);
		}

		public void Break(BreakageParameters breakageParams)
		{
			NativeMethods.Entity.BreakIntoPieces(entity.GetEntityHandle().Handle, 0, 0, breakageParams);
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
			NativeMethods.Physics.Physicalize(entity.GetEntityHandle().Handle, _params);
		}

		/// <summary>
		/// Clears the current physics settings.
		/// </summary>
		public void Clear()
		{
			_params = new PhysicalizationParams 
			{
				copyJointVelocities = false,
				density = -1,
				stiffnessScale = 0,
				mass = -1,
				attachToPart = -1,
				lod = 0,
				slot = -1,
				type = 0,
				attachToEntity = 0,
				flagsOR = 0,
				flagsAND = int.MaxValue,

				playerDim = pe_player_dimensions.Create(),
				playerDyn = pe_player_dynamics.Create()
			};
		}

		public void AddImpulse(Vec3 vImpulse, Vec3? angImpulse = null, Vec3? point = null)
		{
			var impulse = pe_action_impulse.Create();

			impulse.impulse = vImpulse;

			if (angImpulse != null)
				impulse.angImpulse = angImpulse.Value;
			if (point != null)
				impulse.point = point.Value;

			NativeMethods.Physics.AddImpulse(entity.GetEntityHandle().Handle, impulse);
		}

		/// <summary>
		/// Determines if this physical entity is in a sleeping state or not. (Will not be affected by gravity)
		/// Autoamtically wakes upon collision.
		/// </summary>
		public bool Resting
		{
			get { throw new NotImplementedException(); }
			set { NativeMethods.Physics.Sleep(entity.GetEntityHandle().Handle, value); }
		}

		/// <summary>
		/// The mass of the entity in kg.
		/// </summary>
		public float Mass
		{
			get { throw new NotImplementedException(); }
			set { _params.mass = value; _params.density = -1; if(AutoUpdate) Save(); }
		}

		public float Density
		{
			get { throw new NotImplementedException(); }
			set { _params.density = value; _params.mass = -1; if(AutoUpdate) Save(); }
		}

		/// <summary>
		/// The entity slot for which these physical parameters apply.
		/// </summary>
		public int Slot
		{
			get { throw new NotImplementedException(); }
			set { _params.slot = value; if(AutoUpdate) Save(); }
		}

		public PhysicalizationType Type
		{
			get { throw new NotImplementedException(); }
			set { _params.type = value; if(AutoUpdate) Save(); }
		}

		public PhysicalizationFlags FlagsOR
		{
			get { return (PhysicalizationFlags)_params.flagsOR; }
			set { _params.flagsOR = (int)value; if (AutoUpdate) Save(); }
		}

		public PhysicalizationFlags FlagsAND
		{
			get { return (PhysicalizationFlags)_params.flagsAND; }
			set { _params.flagsAND = (int)value; if (AutoUpdate) Save(); }
		}

		/// <summary>
		/// For characters: the scale of force in joint springs.
		/// </summary>
		public float Stiffness
		{
			get { throw new NotImplementedException(); }
			set { _params.stiffnessScale = value; if (AutoUpdate) Save(); }
		}

		#region Temporary workaround to get player dynamics / dimensions working
		public float HeightCollider
		{
			get { throw new NotImplementedException(); }
			set { Debug.LogAlways("1"); _params.playerDim.heightCollider = value; if (AutoUpdate) Save(); }
		}

		public Vec3 SizeCollider
		{
			get { throw new NotImplementedException(); }
			set { Debug.LogAlways("1"); _params.playerDim.sizeCollider = value; if (AutoUpdate) Save(); }
		}

		public float HeightPivot
		{
			get { throw new NotImplementedException(); }
			set { Debug.LogAlways("1"); _params.playerDim.heightPivot = value; if (AutoUpdate) Save(); }
		}

		public bool UseCapsule
		{
			get { throw new NotImplementedException(); }
			set { Debug.LogAlways("1"); _params.playerDim.bUseCapsule = (value == true ? 1 : 0); if (AutoUpdate) Save(); }
		}

		public Vec3 Gravity
		{
			get { throw new NotImplementedException(); }
			set { Debug.LogAlways("1"); _params.playerDyn.gravity = value; if (AutoUpdate) Save(); }
		}

		public float AirControl
		{
			get { throw new NotImplementedException(); }
			set { Debug.LogAlways("1"); _params.playerDyn.kAirControl = value; if (AutoUpdate) Save(); }
		}

		public float MinSlideAngle
		{
			get { throw new NotImplementedException(); }
			set { Debug.LogAlways("1"); _params.playerDyn.minSlideAngle = value; if (AutoUpdate) Save(); }
		}

		public float MaxClimbAngle
		{
			get { throw new NotImplementedException(); }
			set { Debug.LogAlways("1"); _params.playerDyn.maxClimbAngle = value; if (AutoUpdate) Save(); }
		}

		public float MinFallAngle
		{
			get { throw new NotImplementedException(); }
			set { Debug.LogAlways("1"); _params.playerDyn.minFallAngle = value; if (AutoUpdate) Save(); }
		}

		public float MaxVelGround
		{
			get { throw new NotImplementedException(); }
			set { Debug.LogAlways("1"); _params.playerDyn.maxVelGround = value; if (AutoUpdate) Save(); }
		}
		#endregion
		#endregion

		public pe_status_living LivingStatus { get { return NativeMethods.Physics.GetLivingEntityStatus(entity.GetEntityHandle().Handle); } }

		internal IntPtr PhysicsPointer { get; set; }
		public EntityBase entity;

		// Sent directly to the engine
		internal PhysicalizationParams _params;
	}

	internal struct pe_player_dynamics
	{
		public static pe_player_dynamics Create()
		{
			return NativeMethods.Physics.GetPlayerDynamicsStruct();
		}

		public int type;

		public float kInertia;	// inertia koefficient, the more it is, the less inertia is; 0 means no inertia
		public float kInertiaAccel; // inertia on acceleration
		public float kAirControl; // air control koefficient 0..1, 1 - special value (total control of movement)
		public float kAirResistance;	// standard air resistance 
		public Vec3 gravity; // gravity vector
		public float nodSpeed;	// vertical camera shake speed after landings
		public int bSwimming; // whether entity is swimming (is not bound to ground plane)
		public float mass;	// mass (in kg)
		public int surface_idx; // surface identifier for collisions
		public float minSlideAngle; // if surface slope is more than this angle, player starts sliding (angle is in radians)
		public float maxClimbAngle; // player cannot climb surface which slope is steeper than this angle
		public float maxJumpAngle; // player is not allowed to jump towards ground if this angle is exceeded
		public float minFallAngle;	// player starts falling when slope is steeper than this
		public float maxVelGround; // player cannot stand of surfaces that are moving faster than this
		public float timeImpulseRecover; // forcefully turns on inertia for that duration after receiving an impulse
		public int collTypes; // entity types to check collisions against
		IntPtr livingEntToIgnore;
		int bNetwork; // uses extended history information (obsolete)
		int bActive; // 0 disables all simulation for the character, apart from moving along the requested velocity
		int iRequestedTime; // requests that the player rolls back to that time and re-exucutes pending actions during the next step
	}

	internal struct pe_player_dimensions
	{
		public static pe_player_dimensions Create()
		{
			return NativeMethods.Physics.GetPlayerDimensionsStruct();
		}

		public int type;

		public float heightPivot; // offset from central ground position that is considered entity center
		public float heightEye; // vertical offset of camera
		public Vec3 sizeCollider; // collision cylinder dimensions
		public float heightCollider;	// vertical offset of collision geometry center
		public float headRadius;	// radius of the 'head' geometry (used for camera offset)
		public float heightHead;	// center.z of the head geometry
		public Vec3 dirUnproj;	// unprojection direction to test in case the new position overlaps with the environment (can be 0 for 'auto')
		public float maxUnproj; // maximum allowed unprojection
		public int bUseCapsule; // switches between capsule and cylinder collider geometry
	}

	internal struct pe_action_impulse
	{
		public static pe_action_impulse Create()
		{
			return NativeMethods.Physics.GetImpulseStruct();
		}

		public int type;
		public Vec3 impulse;
		public Vec3 angImpulse;	// optional
		public Vec3 point; // point of application, in world CS, optional
		public int partid;	// receiver part identifier
		public int ipart; // alternatively, part index can be used
		public int iApplyTime; // 0-apply immediately, 1-apply before the next time step, 2-apply after the next time step
		public int iSource; // reserved for internal use
	}

	public struct pe_status_living
	{
		public int type;

		public int bFlying; // whether entity has no contact with ground
		public float timeFlying; // for how long the entity was flying
		public Vec3 camOffset; // camera offset
		public Vec3 vel; // actual velocity (as rate of position change)
		public Vec3 velUnconstrained; // 'physical' movement velocity
		public Vec3 velRequested;	// velocity requested in the last action
		public Vec3 velGround; // velocity of the object entity is standing on
		public float groundHeight; // position where the last contact with the ground occured
		public Vec3 groundSlope;
		public int groundSurfaceIdx;
		public int groundSurfaceIdxAux; // contact with the ground that also has default collision flags
		public IntPtr pGroundCollider;	// only returns an actual entity if the ground collider is not static
		public int iGroundColliderPart;
		public float timeSinceStanceChange;
		//int bOnStairs; // tries to detect repeated abrupt ground height changes
		public int bStuck;	// tries to detect cases when the entity cannot move as before because of collisions
		IntPtr pLockStep; // internal timestepping lock
		public int iCurTime; // quantised time
		public int bSquashed; // entity is being pushed by heavy objects from opposite directions
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

	internal struct PhysicalizationParams
	{
		public PhysicalizationType type;

		public int flagsOR;
		public int flagsAND;

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
		/// When physicalizing geometry can specify to use physics from different LOD.
		/// Used for characters that have ragdoll physics in Lod1
		/// </summary>
		public int lod;

		/// <summary>
		/// Physical entity to attach this physics object (Only for Soft physical entity).
		/// </summary>
		public uint attachToEntity;

		/// <summary>
		/// Part ID in entity to attach to (Only for Soft physical entity).
		/// </summary>
		public int attachToPart;

		/// <summary>
		/// Used for character physicalization (Scale of force in character joint's springs).
		/// </summary>
		public float stiffnessScale;

		/// <summary>
		/// Copy joints velocities when converting a character to ragdoll.
		/// </summary>
		public bool copyJointVelocities;

		public pe_player_dimensions playerDim;
		public pe_player_dynamics playerDyn;
	}

	[Flags]
	public enum PhysicalizationFlags
	{
		PushableByPlayers = 0x200,
		FixedDamping = 0x40000,
		NeverBreak = 0x40,
		MonitorPostStep = 0x80000,
		PlayersCanBreak = 0x400000,
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