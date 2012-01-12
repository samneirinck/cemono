using System;

using CryEngine;

/// <summary>
/// GameRulesBase allows new sets of GameRules to be implemented without having to adhere
/// to all the contracts set in IGameRules. Each desired method should be overriden in the
/// child GameRules class.
/// See the Scripts/GameRules/Campaign.cs code that ships with CryMono for example usage.
/// </summary>
[ExcludeFromCompilation]
public class GameRulesBase : BaseGameRules
{
    public GameRulesBase(UInt32 entityId)
        : base(entityId)
    {
    }

    public GameRulesBase() { }

    public override void OnSpawn()
    {
        GameRules._InitHits();
    }

    // Client / Server shared
	public virtual void PrecacheLevel()
	{
		CryConsole.LogAlways("PrecacheLevel");
		string[] items = { "OffHand" };

		foreach(var item in items)
		{
			//ItemSystem.CacheItemGeometry(item);
			//ItemSystem.CacheItemSound(item);
		}
	}

	public virtual void RequestSpawnGroup(UInt32 spawnGroupId)
	{
		CryConsole.LogAlways("RequestSpawnGroup");
	}

	public virtual void SetPlayerSpawnGroup(UInt32 playerId, UInt32 spawnGroupId)
	{
		CryConsole.LogAlways("SetPlayerSpawnGroup");
	}

	public virtual UInt32 GetPlayerSpawnGroup(UInt32 actorId)
	{
		CryConsole.LogAlways("GetPlayerSpawnGroup");
        return 0;
	}

	public virtual void ShowScores(bool show)
	{
		CryConsole.LogAlways("ShowScores");
	}

	// Server-only
    public virtual void OnClientConnect(int channelId, bool isReset = false, string playerName = "")
	{
		CryConsole.LogAlways("OnClientConnect");
	}

    public virtual void OnClientDisconnect(int channelId)
	{
		CryConsole.LogAlways("OnClientDisconnect");
	}

    public virtual void OnClientEnteredGame(int channelId, uint playerId, bool reset, bool loadingSaveGame)
	{
		CryConsole.LogAlways("OnClientEnteredGame");
	}

	public virtual void OnItemDropped(UInt32 itemId, UInt32 actorId)
	{
		CryConsole.LogAlways("OnItemDropped");
	}

	public virtual void OnItemPickedUp(UInt32 itemId, UInt32 actorId)
	{
		CryConsole.LogAlways("OnItemPickedUp");
	}

	public virtual void SvOnVehicleDestroyed(UInt32 vehicleId)
	{
		CryConsole.LogAlways("SvOnVehicleDestroyed");
	}

	public virtual void SvOnVehicleSubmerged(UInt32 vehicleId, float ratio)
	{
		CryConsole.LogAlways("SvOnVehicleSubmerged");
	}

	public virtual void OnAddTaggedEntity(UInt32 shooterId, UInt32 targetId)
	{
		CryConsole.LogAlways("OnAddTaggedEntity");
	}

	public virtual void OnChangeSpectatorMode(UInt32 actorId, byte mode, UInt32 targetId, bool resetAll)
	{
		CryConsole.LogAlways("OnChangeSpectatorMode");
	}

	public virtual void RequestSpectatorTarget(UInt32 playerId, int change)
	{
		CryConsole.LogAlways("RequestSpectatorTarget");
	}

	public virtual void OnChangeTeam(UInt32 actorId, int teamId)
	{
		CryConsole.LogAlways("OnChangeTeam");
	}

	public virtual void OnClientSetTeam(UInt32 actorId, int teamId)
	{
		CryConsole.LogAlways("OnClientSetTeam");
	}

	public virtual void OnSpawnGroupInvalid(UInt32 playerId, UInt32 spawnGroupId)
	{
		CryConsole.LogAlways("OnSpawnGroupInvalid");
	}

	public virtual void RestartGame(bool forceInGame)
	{
		CryConsole.LogAlways("RestartGame");
	}

	// Client-only
	public virtual void OnConnect()
	{
		CryConsole.LogAlways("OnConnect");
	}

	public virtual void OnDisconnect(int cause, string desc)
	{
		CryConsole.LogAlways("OnDisconnect");
	}

	public virtual void OnRevive(UInt32 actorId, Vec3 pos, Vec3 rot, int teamId)
	{
		CryConsole.LogAlways("OnRevive pos: {0} {1} {2} rot: {3} {4} {5}", pos.X.ToString(), pos.Y.ToString(), pos.Z.ToString(), rot.X.ToString(), rot.Y.ToString(), rot.Z,ToString());

        //ItemSystem.GiveItem(actorId, "R870");
	}

	public virtual void OnReviveInVehicle(UInt32 actorId, UInt32 vehicleId, int seatId, int teamId)
	{
		CryConsole.LogAlways("OnReviveInVehicle");
	}

	public virtual void OnKill(UInt32 actorId, UInt32 shooterId, string weaponClassName, int damage, int material, int hit_type)
	{
		CryConsole.LogAlways("OnKill");
	}

	public virtual void OnSetTeam(UInt32 actorId, int teamId)
	{
		CryConsole.LogAlways("OnSetTeam");
	}

	public virtual void OnVehicleDestroyed(UInt32 vehicleId)
	{
		CryConsole.LogAlways("OnVehicleDestroyed");
	}

	public virtual void OnVehicleSubmerged(UInt32 vehicleId, float ratio)
	{
		CryConsole.LogAlways("OnVehicleSubmerged");
	}
}
