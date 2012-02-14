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

        public override void OnClientEnteredGame(int channelId, uint playerId, bool reset, bool loadingSaveGame)
        {
            BasePlayer player = GameRules.GetPlayer(playerId);

            base.OnClientEnteredGame(channelId, playerId, reset, loadingSaveGame);
        }

		public override void OnUpdate()
		{
			// TODO: Register instantiated Players with CryScriptCompiler to utilize its update functionality.
			foreach (var player in GameRules.Players.Where(x => x.ReceiveUpdates))
				player.OnUpdate();
		}

        public override void OnClientConnect(int channelId, bool isReset = false, string playerName = "")
        {
            Player player = GameRules.SpawnPlayer<Player>(channelId, "Player", new Vec3(0, 0, 0), new Vec3(0, 0, 0));

            if (player == null)
            {
                Console.Log("OnClientConnect: Failed to spawn the player!");
                return;
            }

            StaticEntity[] spawnPoints = EntitySystem.GetEntities("SpawnPoint");
            if (spawnPoints == null)
                return;

            if (spawnPoints.Length > 0)
            {
                StaticEntity spawnPoint = spawnPoints[0];
                if (spawnPoint != null)
                {
                    player.Position = spawnPoint.Position;
                    player.Rotation = spawnPoint.Rotation;

                    return;
                }

            }

            Console.LogAlways("$1warning: No spawn points; using default spawn location!");
        }
	}
}