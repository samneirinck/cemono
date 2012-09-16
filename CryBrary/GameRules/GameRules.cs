using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using CryEngine.Initialization;
using CryEngine.Extensions;
using CryEngine.Native;

namespace CryEngine
{
	/// <summary>
	/// This is the base GameRules interface. All game rules must implement this.
	/// </summary>
	/// <remarks>For most use cases, deriving from CryGameCode's BaseGameRules is a more efficient solution.</remarks>
	public abstract class GameRules : EntityBase
	{
		internal void InternalInitialize()
		{
			GameRules.Current = this;

			Id = 1;
			this.SetEntityHandle(new HandleRef(this, NativeMethods.Entity.GetEntity(Id)));
		}

		public static GameRules Current { get; internal set; }
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