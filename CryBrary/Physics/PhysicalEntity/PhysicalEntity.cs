using System;
using System.Linq;
using System.Collections.Generic;

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
        #region Statics
        internal static PhysicalEntity TryGet(IntPtr IPhysicalEntityHandle)
        {
#if !(RELEASE && RELEASE_DISABLE_CHECKS)
            if (IPhysicalEntityHandle == IntPtr.Zero)
                throw new NullPointerException();
#endif

            var physicalEntity = PhysicalEntities.FirstOrDefault(x => x.Handle == IPhysicalEntityHandle);
            if (physicalEntity == null)
            {
                switch (NativePhysicsMethods.GetPhysicalEntityType(IPhysicalEntityHandle))
                {
                    case PhysicalizationType.Static:
                    case PhysicalizationType.Rigid:
                    case PhysicalizationType.WheeledVehicle:
                    case PhysicalizationType.Articulated:
                    case PhysicalizationType.Soft:
                    case PhysicalizationType.Rope:
                        physicalEntity = new PhysicalEntity(IPhysicalEntityHandle);
                        break;
                    case PhysicalizationType.Living:
                        physicalEntity = new PhysicalEntityLiving(IPhysicalEntityHandle);
                        break;
                    case PhysicalizationType.Particle:
                        physicalEntity = new PhysicalEntityParticle(IPhysicalEntityHandle);
                        break;
                    case PhysicalizationType.Area:
                        physicalEntity = new PhysicalEntityArea(IPhysicalEntityHandle);
                        break;
                }

                if(physicalEntity != null)
                    PhysicalEntities.Add(physicalEntity);
            }

            return physicalEntity;
        }

        static List<PhysicalEntity> PhysicalEntities = new List<PhysicalEntity>();
        #endregion

        protected PhysicalEntity() {}

        protected PhysicalEntity(IntPtr physEntPtr)
        {
            Handle = physEntPtr;
        }

        public void Break(BreakageParameters breakageParams)
        {
            NativeEntityMethods.BreakIntoPieces(Owner.GetIEntity(), 0, 0, breakageParams);
        }

        public void AddImpulse(Vec3 vImpulse, Vec3? angImpulse = null, Vec3? point = null)
        {
            var impulse = pe_action_impulse.Create();

            impulse.impulse = vImpulse;

            if (angImpulse != null)
                impulse.angImpulse = angImpulse.Value;
            if (point != null)
                impulse.point = point.Value;

            NativePhysicsMethods.AddImpulse(Owner.GetIEntity(), impulse);
        }

        /// <summary>
        /// Determines if this physical entity is in a sleeping state or not. (Will not be affected by gravity)
        /// Autoamtically wakes upon collision.
        /// </summary>
        public bool Resting
        {
            get { throw new NotImplementedException(); }
            set { NativePhysicsMethods.Sleep(Owner.GetIEntity(), value); }
        }

        public virtual PhysicalizationType Type { get { return NativePhysicsMethods.GetPhysicalEntityType(Handle); } }

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
    }

    public struct PlayerDynamics
    {
        public static PlayerDynamics Create()
        {
            var dyn = new PlayerDynamics();

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
            dyn.collTypes = (EntityQueryFlags)UnusedMarker.Integer;
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

        internal int type;

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
        public EntityQueryFlags collTypes; // entity types to check collisions against
        IntPtr livingEntToIgnore;
        int bNetwork; // uses extended history information (obsolete)
        int bActive; // 0 disables all simulation for the character, apart from moving along the requested velocity
        int iRequestedTime; // requests that the player rolls back to that time and re-exucutes pending actions during the next step
    }

    public struct PlayerDimensions
    {
        internal static PlayerDimensions Create()
        {
            var dim = new PlayerDimensions();

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

        internal int type;

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