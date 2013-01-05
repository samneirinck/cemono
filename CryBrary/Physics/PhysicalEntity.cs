using System;

using System.Runtime.InteropServices;

using CryEngine.Utilities;

using CryEngine.Physics.Status;
using CryEngine.Physics.Actions;

using CryEngine.Native;

namespace CryEngine.Physics
{
    /// <summary>
    /// Physical entity present in the physics system.
    /// </summary>
    public class PhysicalEntity
    {
        private PhysicalEntity() 
        {
            Clear();

            AutoUpdate = true;
        }

        internal PhysicalEntity(IntPtr physEntPtr)
            : this()
        {
            Handle = physEntPtr;
        }

        internal PhysicalEntity(EntityBase _entity)
            : this()
        {
            owner = _entity;
            Handle = NativePhysicsMethods.GetPhysicalEntity(Owner.GetIEntityHandle());
        }

        public void Break(BreakageParameters breakageParams)
        {
            NativeEntityMethods.BreakIntoPieces(Owner.GetIEntityHandle(), 0, 0, breakageParams);
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
            NativePhysicsMethods.Physicalize(Owner.GetIEntityHandle(), _params);
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

            NativePhysicsMethods.AddImpulse(Owner.GetIEntityHandle(), impulse);
        }

        /// <summary>
        /// Determines if this physical entity is in a sleeping state or not. (Will not be affected by gravity)
        /// Autoamtically wakes upon collision.
        /// </summary>
        public bool Resting
        {
            get { throw new NotImplementedException(); }
            set { NativePhysicsMethods.Sleep(Owner.GetIEntityHandle(), value); }
        }

        /// <summary>
        /// The mass of the entity in kg.
        /// </summary>
        public float Mass
        {
            get { throw new NotImplementedException(); }
            set { _params.mass = value; _params.density = -1; if (AutoUpdate) Save(); }
        }

        public float Density
        {
            get { throw new NotImplementedException(); }
            set { _params.density = value; _params.mass = -1; if (AutoUpdate) Save(); }
        }

        /// <summary>
        /// The entity slot for which these physical parameters apply.
        /// </summary>
        public int Slot
        {
            get { throw new NotImplementedException(); }
            set { _params.slot = value; if (AutoUpdate) Save(); }
        }

        public PhysicalizationType Type
        {
            get { throw new NotImplementedException(); }
            set { _params.type = value; if (AutoUpdate) Save(); }
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
            set { _params.playerDim.heightCollider = value; if (AutoUpdate) Save(); }
        }

        public Vec3 SizeCollider
        {
            get { throw new NotImplementedException(); }
            set { _params.playerDim.sizeCollider = value; if (AutoUpdate) Save(); }
        }

        public float HeightPivot
        {
            get { throw new NotImplementedException(); }
            set { _params.playerDim.heightPivot = value; if (AutoUpdate) Save(); }
        }

        public bool UseCapsule
        {
            get { throw new NotImplementedException(); }
            set { _params.playerDim.bUseCapsule = (value == true ? 1 : 0); if (AutoUpdate) Save(); }
        }

        public Vec3 Gravity
        {
            get { throw new NotImplementedException(); }
            set { _params.playerDyn.gravity = value; if (AutoUpdate) Save(); }
        }

        public float AirControl
        {
            get { throw new NotImplementedException(); }
            set { _params.playerDyn.kAirControl = value; if (AutoUpdate) Save(); }
        }

        public float MinSlideAngle
        {
            get { throw new NotImplementedException(); }
            set { _params.playerDyn.minSlideAngle = value; if (AutoUpdate) Save(); }
        }

        public float MaxClimbAngle
        {
            get { throw new NotImplementedException(); }
            set { _params.playerDyn.maxClimbAngle = value; if (AutoUpdate) Save(); }
        }

        public float MinFallAngle
        {
            get { throw new NotImplementedException(); }
            set { _params.playerDyn.minFallAngle = value; if (AutoUpdate) Save(); }
        }

        public float MaxVelGround
        {
            get { throw new NotImplementedException(); }
            set { _params.playerDyn.maxVelGround = value; if (AutoUpdate) Save(); }
        }
        #endregion
        #endregion

        PhysicalStatus status;
        public PhysicalStatus Status 
        { 
            get 
            {
                if(status == null)
                    status = new PhysicalStatus(this); 

                return status;
            }
        }

        /// <summary>
        /// IPhysicalEntity *
        /// </summary>
        internal IntPtr Handle { get; set; }

        private EntityBase owner;
        public EntityBase Owner
        {
            get
            {
                if (owner == null)
                    owner = Entity.Get(NativeEntityMethods.GetEntityFromPhysics(Handle));

                return owner;
            }
        }

        // Sent directly to the engine8
        internal PhysicalizationParams _params;
    }

    internal struct pe_player_dynamics
    {
        public static pe_player_dynamics Create()
        {
            var dyn = new pe_player_dynamics();

            dyn.type = 4;

            dyn.kInertia = UnusedMarker.Float;
            dyn.kInertiaAccel = UnusedMarker.Float;
            dyn.kAirControl = UnusedMarker.Float;
            dyn.gravity = UnusedMarker.Vec3;
            dyn.nodSpeed = UnusedMarker.Float;
            dyn.mass = UnusedMarker.Float;
            dyn.bSwimming = UnusedMarker.Integer;
            dyn.surface_idx = UnusedMarker.Integer;
            dyn.bActive = UnusedMarker.Integer;
            dyn.collTypes = UnusedMarker.Integer;
            dyn.livingEntToIgnore = UnusedMarker.IntPtr;
            dyn.minSlideAngle = UnusedMarker.Float;
            dyn.maxClimbAngle = UnusedMarker.Float;
            dyn.maxJumpAngle = UnusedMarker.Float;
            dyn.minFallAngle = UnusedMarker.Float;
            dyn.kAirResistance = UnusedMarker.Float;
            dyn.bNetwork = UnusedMarker.Integer;
            dyn.maxVelGround = UnusedMarker.Float;
            dyn.timeImpulseRecover = UnusedMarker.Float;
            dyn.iRequestedTime = UnusedMarker.Integer;

            return dyn;
        }

        public int type;

        public float kInertia;    // inertia koefficient, the more it is, the less inertia is; 0 means no inertia
        public float kInertiaAccel; // inertia on acceleration
        public float kAirControl; // air control koefficient 0..1, 1 - special value (total control of movement)
        public float kAirResistance;    // standard air resistance 
        public Vec3 gravity; // gravity vector
        public float nodSpeed;    // vertical camera shake speed after landings
        public int bSwimming; // whether entity is swimming (is not bound to ground plane)
        public float mass;    // mass (in kg)
        public int surface_idx; // surface identifier for collisions
        public float minSlideAngle; // if surface slope is more than this angle, player starts sliding (angle is in radians)
        public float maxClimbAngle; // player cannot climb surface which slope is steeper than this angle
        public float maxJumpAngle; // player is not allowed to jump towards ground if this angle is exceeded
        public float minFallAngle;    // player starts falling when slope is steeper than this
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
            var dim = new pe_player_dimensions();

            dim.type = 1;

            dim.dirUnproj = new Vec3(0, 0, 1);

            dim.sizeCollider = UnusedMarker.Vec3;
            dim.heightPivot = UnusedMarker.Float;
            dim.heightCollider = UnusedMarker.Float;
            dim.heightEye = UnusedMarker.Float;
            dim.heightHead = UnusedMarker.Float;
            dim.headRadius = UnusedMarker.Float;
            dim.bUseCapsule = UnusedMarker.Integer;

            return dim;
        }

        public int type;

        public float heightPivot; // offset from central ground position that is considered entity center
        public float heightEye; // vertical offset of camera
        public Vec3 sizeCollider; // collision cylinder dimensions
        public float heightCollider;    // vertical offset of collision geometry center
        public float headRadius;    // radius of the 'head' geometry (used for camera offset)
        public float heightHead;    // center.z of the head geometry
        public Vec3 dirUnproj;    // unprojection direction to test in case the new position overlaps with the environment (can be 0 for 'auto')
        public float maxUnproj; // maximum allowed unprojection
        public int bUseCapsule; // switches between capsule and cylinder collider geometry
    }
}