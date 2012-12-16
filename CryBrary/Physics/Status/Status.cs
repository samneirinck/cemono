using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using CryEngine.Native;

namespace CryEngine.Physics.Status
{
    public class PhysicalStatus
    {
        public PhysicalStatus(PhysicalEntity physEnt)
        {
            PhysicalEntity = physEnt;
        }

        public LivingPhysicsStatus Living { get { return NativePhysicsMethods.GetLivingEntityStatus(PhysicalEntity.Owner.GetEntityHandle()); } }
        public DynamicsPhysicsStatus Dynamics { get { return NativePhysicsMethods.GetDynamicsEntityStatus(PhysicalEntity.Owner.GetEntityHandle()); } }

        private PhysicalEntity PhysicalEntity { get; set; }
    }
}
