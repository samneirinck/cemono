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
            Console.LogAlways("Player.OnSpawn");

            Inventory.GiveEquipmentPack("Singleplayer");
        }
    }
}
