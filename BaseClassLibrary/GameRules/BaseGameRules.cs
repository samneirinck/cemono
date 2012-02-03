using System;

namespace CryEngine
{
	/// <summary>
	/// This is the base GameRules interface. All game rules must implement this.
	/// </summary>
	/// <remarks>For most use cases, deriving from CryGameCode's BaseGameRules is a more efficient solution.</remarks>
    public abstract class BaseGameRules : CryScriptInstance
    {
        // Unsure
        public virtual void PrecacheLevel() { }
        public virtual void RequestSpawnGroup(uint spawnGroupId) { }
        public virtual void SetPlayerSpawnGroup(uint playerId, uint spawnGroupId) { }
        public virtual uint GetPlayerSpawnGroup(uint actorId) { return Convert.ToUInt32(0); }
        public virtual void ShowScores(bool show) { }

        // Server-only
        public virtual float GetDamageMult(string materialName) { return 1.0f; }

        public virtual void OnSpawn() { }

        public virtual void OnClientConnect(int channelId, bool isReset = false, string playerName = "") { }
        public virtual void OnClientDisconnect(int channelId) { }

        public virtual void OnClientEnteredGame(int channelId, uint playerId, bool reset, bool loadingSaveGame) { }

        public virtual void OnItemDropped(uint itemId, uint actorId) { }
        public virtual void OnItemPickedUp(uint itemId, uint actorId) { }

        public virtual void SvOnVehicleDestroyed(uint vehicleId) { }
        public virtual void SvOnVehicleSubmerged(uint vehicleId, float ratio) { }

        public virtual void OnAddTaggedEntity(uint shooterId, uint targetId) { }

        public virtual void OnChangeSpectatorMode(uint actorId, byte mode, uint targetId, bool resetAll) { }
        public virtual void RequestSpectatorTarget(uint playerId, int change) { }

        public virtual void OnChangeTeam(uint actorId, int teamId) { }
        public virtual void OnClientSetTeam(uint actorId, int teamId) { }

        public virtual void OnSpawnGroupInvalid(uint playerId, uint spawnGroupId) { }

        public virtual void RestartGame(bool forceInGame) { }

        // Client-only
        public virtual void OnConnect() { }
        public virtual void OnDisconnect(int cause, string desc) { }

        public virtual void OnRevive(uint actorId, Vec3 pos, Vec3 rot, int teamId) { }
        public virtual void OnReviveInVehicle(uint actorId, uint vehicleId, int seatId, int teamId) { }
        public virtual void OnKill(uint actorId, uint shooterId, string weaponClassName, int damage, int material, int hit_type) { }

        public virtual void OnSetTeam(uint actorId, int teamId) { }

        public virtual void OnVehicleDestroyed(uint vehicleId) { }
        public virtual void OnVehicleSubmerged(uint vehicleId, float ratio) { }
    }

    public struct HitInfo
    {
        /// <summary>
        /// EntityId of the shooter
        /// </summary>
        public uint shooterId;
        /// <summary>
        /// EntityId of the target which got shot
        /// </summary>
        public uint targetId;
        /// <summary>
        /// EntityId of the weapon
        /// </summary>
        public uint weaponId;
        /// <summary>
        /// 0 if hit was not caused by a projectile
        /// </summary>
        public uint projectileId;

        /// <summary>
        /// damage count of the hit
        /// </summary>
        public float damage;
        public float impulse;
        public float radius;
        /// <summary>
        /// radius of the hit
        /// </summary>
        public float angle;
        /// <summary>
        /// material id of the surface which got hit
        /// </summary>
        public int material;
        /// <summary>
        /// type id of the hit, see IGameRules::GetHitTypeId for more information
        /// </summary>
        public int type;
        /// <summary>
        /// type of bullet, if hit was of type bullet
        /// </summary>
        public int bulletType;

        public float damageMin;
        /// <summary>
        /// bullet pierceability
        /// </summary>
        public float pierce;

        public int partId;

        /// <summary>
        /// position of the hit
        /// </summary>
        public Vec3 pos;
        /// <summary>
        /// direction of the hit
        /// </summary>
        public Vec3 dir;
        public Vec3 normal;

        public UInt16 projectileClassId;
        public UInt16 weaponClassId;

        public bool remote;
        /// <summary>
        /// set to true if shot was aimed - i.e. first bullet, zoomed in etc.
        /// </summary>
        public bool aimed;
        /// <summary>
        /// true if the hit should knockdown
        /// </summary>
        public bool knocksDown;
        /// <summary>
        /// true if the hit should knockdown when hit in a leg
        /// </summary>
        public bool knocksDownLeg;
        /// <summary>
        /// true if the 'shooter' didn't actually shoot, ie. a weapon acting on their behalf did (team perks)
        /// </summary>
        public bool hitViaProxy;
        /// <summary>
        /// true if this hit directly results from an explosion
        /// </summary>
        public bool explosion;
        /// <summary>
        /// dynamic pierceability reduction
        /// </summary>
        public float armorHeating;
        /// <summary>
        /// number of surfaces the bullet has penetrated
        /// </summary>
        public int penetrationCount;
    }
}