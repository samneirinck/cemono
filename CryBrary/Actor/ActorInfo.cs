using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CryEngine
{
    internal struct ActorInitializationParams : IScriptInitializationParams
    {
        public IntPtr EntityPtr;
        public IntPtr ActorPtr;
        public uint Id;
        public int ChannelId;
    }
}
