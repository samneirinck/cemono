using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using CryEngine.Native;
using CryEngine.Utilities;

namespace CryEngine.Physics.Actions
{
    internal struct pe_action_impulse
    {
        public static pe_action_impulse Create()
        {
            var action = new pe_action_impulse();

            action.type = 2;
            action.iApplyTime = 2;

            action.point = UnusedMarker.Vec3;
            action.angImpulse = UnusedMarker.Vec3;
            action.partid = UnusedMarker.Integer;
            action.ipart = UnusedMarker.Integer;

            return action;
        }

        public int type;

        public Vec3 impulse;
        public Vec3 angImpulse;    // optional
        public Vec3 point; // point of application, in world CS, optional
        public int partid;    // receiver part identifier
        public int ipart; // alternatively, part index can be used
        public int iApplyTime; // 0-apply immediately, 1-apply before the next time step, 2-apply after the next time step
        public int iSource; // reserved for internal use
    }
}
