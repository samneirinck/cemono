using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CryEngine.Physics
{
    public class PhysicalEntityLiving : PhysicalEntity
    {
        internal PhysicalEntityLiving(IntPtr physEntPtr)
        {
            Handle = physEntPtr;
        }

        public override PhysicalizationType Type
        {
            get { return PhysicalizationType.Living; }
        }
    }
}
