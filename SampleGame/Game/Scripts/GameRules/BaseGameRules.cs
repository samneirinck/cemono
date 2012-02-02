using System.IO;
using System.Collections.Generic;
//using System.Xml.Linq;

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
    public GameRulesBase() 
    {
        m_damageTable = new Dictionary<string, float>(); 
    }

    public override void OnSpawn()
    {
        //XDocument damageTable = XDocument.Load(Path.Combine(CryPath.GetScriptsFolder(), @"GameRules\DamageTable.xml"));

       // foreach (XElement body in damageTable.Root.Nodes())
            //m_damageTable.Add(body.Attribute("part").Value, Convert.ToSingle(body.Attribute("damageMultiplier").Value));
    }

    // Client / Server shared
	public override void PrecacheLevel()
	{
		Console.LogAlways("PrecacheLevel");
		string[] items = { "OffHand" };

		foreach(var item in items)
		{
			//ItemSystem.CacheItemGeometry(item);
			//ItemSystem.CacheItemSound(item);
		}
	}

    public override void RequestSpawnGroup(uint spawnGroupId)
	{
		Console.LogAlways("RequestSpawnGroup");
	}

    public override void SetPlayerSpawnGroup(uint playerId, uint spawnGroupId)
	{
		Console.LogAlways("SetPlayerSpawnGroup");
	}

    public override uint GetPlayerSpawnGroup(uint actorId)
	{
		Console.LogAlways("GetPlayerSpawnGroup");
        return 0;
	}

    public override void ShowScores(bool show)
	{
		Console.LogAlways("ShowScores");
	}

	// Server-only
    public override float GetDamageMult(string materialType)
    {
        Console.LogAlways("GetDamageMult");

        if (m_damageTable.ContainsKey(materialType))
            return m_damageTable[materialType];

        Console.LogAlways("ret");

        return 1.0f;
    }

    public override void OnClientConnect(int channelId, bool isReset = false, string playerName = "")
	{
		Console.LogAlways("OnClientConnect");
	}

    public override void OnClientDisconnect(int channelId)
	{
		Console.LogAlways("OnClientDisconnect");
	}

    public override void OnClientEnteredGame(int channelId, uint playerId, bool reset, bool loadingSaveGame)
	{
		Console.LogAlways("OnClientEnteredGame");
	}

    public override void OnItemDropped(uint itemId, uint actorId)
	{
		Console.LogAlways("OnItemDropped");
	}

    public override void OnItemPickedUp(uint itemId, uint actorId)
	{
		Console.LogAlways("OnItemPickedUp");
	}

    public override void SvOnVehicleDestroyed(uint vehicleId)
	{
		Console.LogAlways("SvOnVehicleDestroyed");
	}

    public override void SvOnVehicleSubmerged(uint vehicleId, float ratio)
	{
		Console.LogAlways("SvOnVehicleSubmerged");
	}

    public override void OnAddTaggedEntity(uint shooterId, uint targetId)
	{
		Console.LogAlways("OnAddTaggedEntity");
	}

    public override void OnChangeSpectatorMode(uint actorId, byte mode, uint targetId, bool resetAll)
	{
		Console.LogAlways("OnChangeSpectatorMode");
	}

	public override void RequestSpectatorTarget(uint playerId, int change)
	{
		Console.LogAlways("RequestSpectatorTarget");
	}

	public override void OnChangeTeam(uint actorId, int teamId)
	{
		Console.LogAlways("OnChangeTeam");
	}

	public override void OnClientSetTeam(uint actorId, int teamId)
	{
		Console.LogAlways("OnClientSetTeam");
	}

	public override void OnSpawnGroupInvalid(uint playerId, uint spawnGroupId)
	{
		Console.LogAlways("OnSpawnGroupInvalid");
	}

	public override void RestartGame(bool forceInGame)
	{
		Console.LogAlways("RestartGame");
	}

	// Client-only
	public override void OnConnect()
	{
		Console.LogAlways("OnConnect");
	}

	public override void OnDisconnect(int cause, string desc)
	{
		Console.LogAlways("OnDisconnect");
	}

	public override void OnRevive(uint actorId, Vec3 pos, Vec3 rot, int teamId)
	{
		Console.LogAlways("OnRevive pos: {0} {1} {2} rot: {3} {4} {5}", pos.X.ToString(), pos.Y.ToString(), pos.Z.ToString(), rot.X.ToString(), rot.Y.ToString(), rot.Z,ToString());

        //ItemSystem.GiveItem(actorId, "R870");
	}

	public override void OnReviveInVehicle(uint actorId, uint vehicleId, int seatId, int teamId)
	{
		Console.LogAlways("OnReviveInVehicle");
	}

	public override void OnKill(uint actorId, uint shooterId, string weaponClassName, int damage, int material, int hit_type)
	{
		Console.LogAlways("OnKill");
	}

	public override void OnSetTeam(uint actorId, int teamId)
	{
		Console.LogAlways("OnSetTeam");
	}

	public override void OnVehicleDestroyed(uint vehicleId)
	{
		Console.LogAlways("OnVehicleDestroyed");
	}

	public override void OnVehicleSubmerged(uint vehicleId, float ratio)
	{
		Console.LogAlways("OnVehicleSubmerged");
	}

    Dictionary<string /* material type */, float /* damage mult */> m_damageTable;
}
