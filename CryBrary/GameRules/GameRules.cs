using System;
using System.Runtime.CompilerServices;

using CryEngine.Initialization;
using CryEngine.Extensions;
using CryEngine.Native;

namespace CryEngine
{
	/// <summary>
	/// This is the base GameRules interface. All game rules must implement this.
	/// </summary>
	/// <remarks>For most use cases, deriving from CryGameCode's BaseGameRules is a more efficient solution.</remarks>
	public abstract class GameRules : CryScriptInstance
	{
        #region Statics
		public static GameRules Current { get; internal set; }
		#endregion

		// Shared
		public virtual void PrecacheLevel() { }
		public virtual void RequestSpawnGroup(EntityId spawnGroupId) { }
		public virtual void SetPlayerSpawnGroup(EntityId playerId, EntityId spawnGroupId) { }
		public virtual EntityId GetPlayerSpawnGroup(EntityId actorId) { return new EntityId(System.Convert.ToUInt32(0)); }
		public virtual void ShowScores(bool show) { }

		public virtual void OnSetTeam(EntityId actorId, EntityId teamId) { }

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

	[AttributeUsage(AttributeTargets.Class)]
	public sealed class GameRulesAttribute : Attribute
	{
		/// <summary>
		/// Sets the game mode's name. Uses the class name if not set.
		/// </summary>
		public string Name { get; set; }

		/// <summary>
		/// If set to true, the game mode will be set as default.
		/// </summary>
		public bool Default { get; set; }
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
}