using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CryEngine
{
    [Flags]
    public enum PhysicalizationFlags
    {
        PushableByPlayers = 0x200,
        FixedDamping = 0x40000,
        NeverBreak = 0x40,
        MonitorPostStep = 0x80000,
        PlayersCanBreak = 0x400000,
    }
}
