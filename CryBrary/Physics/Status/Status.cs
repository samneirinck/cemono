using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using CryEngine.Native;

namespace CryEngine.Physics.Status
{
    public class PhysicalStatus
    {
        PhysicalStatus() { }

        public PhysicalStatus(PhysicalEntity physEnt)
        {
            PhysicalEntity = physEnt;
        }

        public LivingPhysicsStatus Living { get { return NativePhysicsMethods.GetLivingEntityStatus(PhysicalEntity.Owner.GetIEntity()); } }
        public DynamicsPhysicsStatus Dynamics { get { return NativePhysicsMethods.GetDynamicsEntityStatus(PhysicalEntity.Owner.GetIEntity()); } }

        private PhysicalEntity PhysicalEntity { get; set; }
    }
}
