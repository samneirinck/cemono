using System;

using CryEngine;

/// <summary>
/// The campaign game mode is the base game mode
/// </summary>
namespace CryGameCode
{
    [DefaultGamemodeAttribute]
	public class SinglePlayer : GameRulesBase
	{
        public SinglePlayer(UInt32 entityId)
            : base(entityId)
        {
        }

        public SinglePlayer() { }

        public override void OnClientEnteredGame(int channelId, uint playerId, bool reset, bool loadingSaveGame)
        {
            BasePlayer player = GameRules.GetPlayer(playerId);

            base.OnClientEnteredGame(channelId, playerId, reset, loadingSaveGame);
        }

        public override void OnClientConnect(int channelId, bool isReset = false, string playerName = "")
        {
            Player player = GameRules.SpawnPlayer<Player>(channelId, "Dude", new Vec3(0, 0, 0), new Vec3(0, 0, 0));

            if (player == null)
            {
                CryConsole.Log("OnClientConnect: Failed to spawn the player!");
                return;
            }

            player.OnSpawn();

            BaseEntity[] spawnPoints = EntitySystem.GetEntities("SpawnPoint");
            if (spawnPoints == null)
                return;

            if (spawnPoints.Length > 0)
            {
                BaseEntity spawnPoint = spawnPoints[0];

                if (spawnPoint != null)
                {
                    player.Position = spawnPoint.Position;
                    player.Rotation = spawnPoint.Rotation;

                    return;
                }
            }

            CryConsole.LogAlways("$1warning: No spawn points; using default spawn location!");
        }
	}
}