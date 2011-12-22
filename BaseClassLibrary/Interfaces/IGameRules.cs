using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CryEngine
{
	/// <summary>
	/// This is the base GameRules interface. All game rules must implement this.
	/// </summary>
	/// <remarks>For most use cases, deriving from CryGameCode's BaseGameRules is a more efficient solution.</remarks>
    public interface IGameRules : ICryScriptType
    {
        // Unsure
        void PrecacheLevel();
        void RequestSpawnGroup(UInt32 spawnGroupId);
        void SetPlayerSpawnGroup(UInt32 playerId, UInt32 spawnGroupId);
        UInt32 GetPlayerSpawnGroup(UInt32 actorId);
        void ShowScores(bool show);

        // Client / Server shared
        // Server-only
        void OnClientConnect(int channelId, bool isReset = false, string playerName = "");
        void OnClientDisconnect(int channelId);

        void OnClientEnteredGame(int channelId, uint playerId, bool reset, bool loadingSaveGame);

        void OnItemDropped(UInt32 itemId, UInt32 actorId);
        void OnItemPickedUp(UInt32 itemId, UInt32 actorId);

        void SvOnVehicleDestroyed(UInt32 vehicleId);
        void SvOnVehicleSubmerged(UInt32 vehicleId, float ratio);

        void OnAddTaggedEntity(UInt32 shooterId, UInt32 targetId);

        void OnChangeSpectatorMode(UInt32 actorId, byte mode, UInt32 targetId, bool resetAll);
        void RequestSpectatorTarget(UInt32 playerId, int change);

        void OnChangeTeam(UInt32 actorId, int teamId);
        void OnClientSetTeam(UInt32 actorId, int teamId);

        void OnSpawnGroupInvalid(UInt32 playerId, UInt32 spawnGroupId);

        void RestartGame(bool forceInGame);


        // Client-only
        void OnConnect();
        void OnDisconnect(int cause, string desc);

        void OnRevive(UInt32 actorId, Vec3 pos, Vec3 rot, int teamId);
        void OnReviveInVehicle(UInt32 actorId, UInt32 vehicleId, int seatId, int teamId);
        void OnKill(UInt32 actorId, UInt32 shooterId, string weaponClassName, int damage, int material, int hit_type);

        void OnSetTeam(UInt32 actorId, int teamId);

        void OnVehicleDestroyed(UInt32 vehicleId);
        void OnVehicleSubmerged(UInt32 vehicleId, float ratio);
    }
}