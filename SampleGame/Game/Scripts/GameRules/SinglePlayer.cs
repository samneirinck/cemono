using CryEngine;

using System.Linq;

/// <summary>
/// The campaign game mode is the base game mode
/// </summary>
namespace CryGameCode
{
    [DefaultGamemodeAttribute]
	public class SinglePlayer : GameRulesBase
	{
        public SinglePlayer()
        {
			ReceiveUpdates = true;
        }

        public override void OnClientConnect(int channelId, bool isReset = false, string playerName = "")
        {
            Player player = GameRules.SpawnPlayer<Player>(channelId, "Player", new Vec3(0, 0, 0), new Vec3(0, 0, 0));

			if (player == null)
				Console.Log("OnClientConnect: Failed to spawn the player!");
        }

		public override void OnRevive(uint actorId, Vec3 pos, Vec3 rot, int teamId)
		{
			BasePlayer player = GameRules.GetPlayer(actorId);
			if (player == null)
			{
				Console.LogAlways("[SinglePlayer.OnRevive] Failed to get player");
				return;
			}

			StaticEntity[] spawnPoints = EntitySystem.GetEntities("SpawnPoint");
			if (spawnPoints == null || spawnPoints.Length < 1)
				Console.LogAlways("$1warning: No spawn points; using default spawn location!");
			else
			{
				StaticEntity spawnPoint = spawnPoints[0];
				if (spawnPoint != null)
				{
					Console.LogAlways("Found spawnpoint", spawnPoint.Name);

					player.Position = spawnPoint.Position;
					player.Rotation = spawnPoint.Rotation;
				}
			}

			player.Inventory.GiveEquipmentPack("SinglePlayer");
		}
	}
}