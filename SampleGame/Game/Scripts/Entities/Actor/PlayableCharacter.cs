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
			InputSystem.RegisterAction("testAction", OnTestActionTriggered);
        }

        public override void OnSpawn()
        {
            Console.LogAlways("Player.OnSpawn");

            Inventory.GiveEquipmentPack("Singleplayer");
        }

		public void OnTestActionTriggered(InputSystem.ActionActivationMode activationMode, float value)
		{
			Console.LogAlways("TestAction triggered with mode {0} and value {1}", activationMode, value);
		}
    }
}
