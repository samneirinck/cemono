using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using CryEngine;

namespace CryGameCode
{
    public class Player : BasePlayer
    {
        public Player() 
        {
        }

        public override void OnSpawn()
        {
            CryConsole.LogAlways("Player.OnSpawn");

            Inventory.GiveEquipmentPack("SinglePlayer");
        }

		public void VoidRMITest()
		{
			CryConsole.LogAlways("RMI'd, bitch");
		}

		public void ValueRMITest(int inty, string stringy, float floaty)
		{
			CryConsole.LogAlways("RMI'd with values, bitch: {0} | {1} | {2}", inty, stringy, floaty);
		}
    }
}
