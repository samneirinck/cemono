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
			Save();

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
				density = -1,
				mass = -1,
				slot = 0,
				attachToPart = -1,

				heightCollider = Utils.UnusedMarker.Float,
				sizeCollider = Utils.UnusedMarker.Vec3,
				heightPivot = Utils.UnusedMarker.Float,

				gravity = Utils.UnusedMarker.Vec3,
				airControl = Utils.UnusedMarker.Float,
				minSlideAngle = Utils.UnusedMarker.Float,
				maxClimbAngle = Utils.UnusedMarker.Float,
				minFallAngle = Utils.UnusedMarker.Float,
				maxVelGround = Utils.UnusedMarker.Float,
			};
		}

		public void AddImpulse(Vec3 impulse, Vec3? angImpulse = null, Vec3? point = null)
		{
			var actionImpulse = new ActionImpulse 
			{ 
				impulse = impulse, 
				angImpulse = angImpulse ?? Utils.UnusedMarker.Vec3,
				point = point ?? Utils.UnusedMarker.Vec3,
				partid = Utils.UnusedMarker.Integer,
				ipart = Utils.UnusedMarker.Integer,
				iApplyTime = PhysicsApplyTime.PostStep,
				iSource = 0
			};

			NativeMethods.Physics.AddImpulse(entity.GetEntityHandle().Handle, actionImpulse);
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
			set { _params.heightCollider = value; if (AutoUpdate) Save(); }
		}

		public Vec3 SizeCollider
		{
			get { throw new NotImplementedException(); }
			set { _params.sizeCollider = value; if (AutoUpdate) Save(); }
		}

		public float HeightPivot
		{
			get { throw new NotImplementedException(); }
			set { _params.heightPivot = value; if (AutoUpdate) Save(); }
		}

		public bool UseCapsule
		{
			get { throw new NotImplementedException(); }
			set { _params.useCapsule = value; if (AutoUpdate) Save(); }
		}

		public Vec3 Gravity
		{
			get { throw new NotImplementedException(); }
			set { _params.gravity = value; if (AutoUpdate) Save(); }
		}

		public float AirControl
		{
			get { throw new NotImplementedException(); }
			set { _params.airControl = value; if (AutoUpdate) Save(); }
		}

		public float MinSlideAngle
		{
			get { throw new NotImplementedException(); }
			set { _params.minSlideAngle = value; if (AutoUpdate) Save(); }
		}

		public float MaxClimbAngle
		{
			get { throw new NotImplementedException(); }
			set { _params.maxClimbAngle = value; if (AutoUpdate) Save(); }
		}

		public float MinFallAngle
		{
			get { throw new NotImplementedException(); }
			set { _params.minFallAngle = value; if (AutoUpdate) Save(); }
		}

		public float MaxVelGround
		{
			get { throw new NotImplementedException(); }
			set { _params.maxVelGround = value; if (AutoUpdate) Save(); }
		}
		#endregion
		#endregion

		internal IntPtr PhysicsPointer { get; set; }
		public EntityBase entity;

		// Sent directly to the engine
		internal PhysicalizationParams _params;
	}

	internal struct ActionImpulse
	{
		public Vec3 impulse;
		public Vec3 angImpulse;	// optional
		public Vec3 point; // point of application, in world CS, optional 
		public int partid;	// receiver part identifier
		public int ipart; // alternatively, part index can be used
		public PhysicsApplyTime iApplyTime; // 0-apply immediately, 1-apply before the next time step, 2-apply after the next time step
		internal int iSource; // reserved for internal use
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
		/// When physicalizing geometry can specify to use physics from different LOD.
		/// Used for characters that have ragdoll physics in Lod1
		/// </summary>
		public int lod;

		/// <summary>
		/// Physical entity to attach this physics object (Only for Soft physical entity).
		/// </summary>
		uint attachToEntity;

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

		// pe_player_dimensions
		public float heightCollider;
		public Vec3 sizeCollider;
		public float heightPivot;
		public bool useCapsule;

		// pe_player_dynamics
		public Vec3 gravity;
		public float airControl;
		public float minSlideAngle;
		public float maxClimbAngle;
		public float minFallAngle;
		public float maxVelGround;
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