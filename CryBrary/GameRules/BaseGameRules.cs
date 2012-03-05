using System;

namespace CryEngine
{
	/// <summary>
	/// This is the base GameRules interface. All game rules must implement this.
	/// </summary>
	/// <remarks>For most use cases, deriving from CryGameCode's BaseGameRules is a more efficient solution.</remarks>
    public abstract class BaseGameRules : CryScriptInstance
    {
        // Shared
        public virtual void PrecacheLevel() { }
        public virtual void RequestSpawnGroup(EntityId spawnGroupId) { }
		public virtual void SetPlayerSpawnGroup(EntityId playerId, EntityId spawnGroupId) { }
		public virtual EntityId GetPlayerSpawnGroup(EntityId actorId) { return new EntityId(System.Convert.ToUInt32(0)); }
        public virtual void ShowScores(bool show) { }

		public virtual void OnSetTeam(EntityId actorId, EntityId teamId) { }

        // Server-only
		protected virtual void OnHit(DamageInfo hitInfo) { }

        public virtual void OnSpawn() { }

        public virtual void OnClientConnect(int channelId, bool isReset = false, string playerName = "") { }
        public virtual void OnClientDisconnect(int channelId) { }

		public virtual void OnClientEnteredGame(int channelId, EntityId playerId, bool reset, bool loadingSaveGame) { }

		public virtual void OnItemDropped(EntityId itemId, EntityId actorId) { }
		public virtual void OnItemPickedUp(EntityId itemId, EntityId actorId) { }

		public virtual void SvOnVehicleDestroyed(EntityId vehicleId) { }
		public virtual void SvOnVehicleSubmerged(EntityId vehicleId, float ratio) { }

		public virtual void OnAddTaggedEntity(EntityId shooterId, EntityId targetId) { }

		public virtual void OnChangeSpectatorMode(EntityId actorId, byte mode, EntityId targetId, bool resetAll) { }
		public virtual void RequestSpectatorTarget(EntityId playerId, int change) { }

		public virtual void OnChangeTeam(EntityId actorId, int teamId) { }

		public virtual void OnSpawnGroupInvalid(EntityId playerId, EntityId spawnGroupId) { }

        public virtual void RestartGame(bool forceInGame) { }

        // Client-only
        public virtual void OnConnect() { }
		public virtual void OnDisconnect(DisconnectionCause cause, string description) { }

		public virtual void OnRevive(EntityId actorId, Vec3 pos, Vec3 rot, int teamId) { }
		public virtual void OnReviveInVehicle(EntityId actorId, EntityId vehicleId, int seatId, int teamId) { }
		public virtual void OnKill(EntityId actorId, EntityId shooterId, string weaponClassName, int damage, int material, int hitType) { }

		public virtual void OnVehicleDestroyed(EntityId vehicleId) { }
		public virtual void OnVehicleSubmerged(EntityId vehicleId, float ratio) { }
    }

	public enum DisconnectionCause
	{
		/// <summary>
		/// This cause must be first! - timeout occurred.
		/// </summary>
		Timeout = 0,
		/// <summary>
		/// Incompatible protocols.
		/// </summary>
		ProtocolError,
		/// <summary>
		/// Failed to resolve an address.
		/// </summary>
		ResolveFailed,
		/// <summary>
		/// Versions mismatch.
		/// </summary>
		VersionMismatch,
		/// <summary>
		/// Server is full.
		/// </summary>
		ServerFull,
		/// <summary>
		/// User initiated kick.
		/// </summary>
		Kicked,
		/// <summary>
		/// Teamkill ban/ admin ban.
		/// </summary>
		Banned,
		/// <summary>
		/// Context database mismatch.
		/// </summary>
		ContextCorruption,
		/// <summary>
		/// Password mismatch, cdkey bad, etc.
		/// </summary>
		AuthenticationFailed,
		/// <summary>
		/// Misc. game error.
		/// </summary>
		GameError,
		/// <summary>
		/// DX11 not found.
		/// </summary>
		NotDX11Capable,
		/// <summary>
		/// The nub has been destroyed.
		/// </summary>
		NubDestroyed,
		/// <summary>
		/// Icmp reported error.
		/// </summary>
		ICMPError,
		/// <summary>
		/// NAT negotiation error.
		/// </summary>
		NatNegError,
		/// <summary>
		/// Punk buster detected something bad.
		/// </summary>
		PunkDetected,
		/// <summary>
		/// Demo playback finished.
		/// </summary>
		DemoPlaybackFinished,
		/// <summary>
		/// Demo playback file not found.
		/// </summary>
		DemoPlaybackFileNotFound,
		/// <summary>
		/// User decided to stop playing.
		/// </summary>
		UserRequested,
		/// <summary>
		/// User should have controller connected.
		/// </summary>
		NoController,
		/// <summary>
		/// Unable to connect to server.
		/// </summary>
		CantConnect,
		/// <summary>
		/// Arbitration failed in a live arbitrated session.
		/// </summary>
		ArbitrationFailed,
		/// <summary>
		/// Failed to successfully join migrated game
		/// </summary>
		FailedToMigrateToNewHost,
		/// <summary>
		/// The session has just been deleted
		/// </summary>
		SessionDeleted,
		/// <summary>
		/// Unknown cause
		/// </summary>
		Unknown
	}

	public struct DamageInfo
	{
		internal HitInfo _info;

		internal DamageInfo(HitInfo info)
		{
			_info = info;
		}

		public EntityId Shooter { get { return new EntityId(_info.shooterId); } }
		public EntityId Target { get { return new EntityId(_info.targetId); } }

		public Vec3 Position { get { return _info.pos; } }
		public Vec3 Direction { get { return _info.dir; } }
		public float Impulse { get { return _info.impulse; } }

		public float Damage { get { return _info.damage; } }
	}

    internal struct HitInfo
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