using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CryEngine
{
	/// <summary>
	/// WIP Player class.
	/// </summary>
    public class Player : Entity
    {
        public Player(UInt32 entityId, int channelId)
        {
            Id = entityId;
            ChannelId = channelId;
        }

        public int ChannelId { get; set; }
        /*
        public void SetSpectatorMode(int newMode, Entity spectatedEntity = null)
        {
        }

        public int GetSpectatorMode()
        {
            return 0;
        }

        public float Health { get; set; }
        public float MaxHealth { get; set; }

        public bool IsDead() { return Health <= 0; }*/
    }
}
