using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using CryEngine.Physics;

namespace CryEngine
{
    public struct PhysicalizationParams
    {
        public PhysicalizationParams(PhysicalizationType _type)
            : this()
        {
            type = _type;

            flagsAND = (PhysicalizationFlags)int.MaxValue;
            slot = -1;
            density = -1;
            mass = -1;
            attachToPart = -1;

            livingDynamics = PlayerDynamics.Create();
            livingDimensions = PlayerDimensions.Create();
        }

        internal PhysicalizationType type;

        public PhysicalizationFlags flagsOR;
        public PhysicalizationFlags flagsAND;

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
        internal uint attachToEntity;

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

        public PlayerDimensions livingDimensions;
        public PlayerDynamics livingDynamics;
    }
}
