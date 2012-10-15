using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Runtime.InteropServices;

using CryEngine.Native;

namespace CryEngine
{
	public abstract class ActorBase : EntityBase
	{
		/// <summary>
		/// Initializes the player.
		/// </summary>
		/// <param name="actorInfo"></param>
		/// <param name="channelId"></param>
		internal virtual void InternalSpawn(ActorInfo actorInfo, int channelId)
		{
			System.Diagnostics.Contracts.Contract.Requires(channelId > 0);
			Id = new EntityId(actorInfo.Id);
			this.SetActorHandle(new HandleRef(this, actorInfo.ActorPtr));
			this.SetEntityHandle(new HandleRef(this, actorInfo.EntityPtr));

			ChannelId = channelId;

			// actor must have physics
			Physics.Type = PhysicalizationType.Rigid;

			OnSpawn();
		}

		#region Callbacks
		/// <summary>
		/// Called after successful actor creation via Actor.Create.
		/// </summary>
		public virtual void OnSpawn() { }
		#endregion

		#region Overrides
		public override void Remove(bool forceRemoveNow = false)
		{
			if (forceRemoveNow)
				throw new NotSupportedException("forceRemoveNow is not supported for actor types.");

			Actor.Remove(Id);
		}

		public override int GetHashCode()
		{
			unchecked // Overflow is fine, just wrap
			{
				int hash = 17;

				hash = hash * 29 + ScriptId.GetHashCode();
				hash = hash * 29 + Id.GetHashCode();
				hash = hash * 29 + ChannelId.GetHashCode();
				hash = hash * 29 + this.GetActorHandle().Handle.GetHashCode();
				hash = hash * 29 + this.GetEntityHandle().Handle.GetHashCode();

				return hash;
			}
		}

		internal override void OnScriptReloadInternal()
		{
			this.SetActorHandle(new HandleRef(this, NativeMethods.Actor.GetActorInfoById(Id).ActorPtr));

			base.OnScriptReloadInternal();
		}
		#endregion

		/// <summary>
		/// Checks if this actor is controlled by the local client. See <see cref="Actor.LocalClient"/>.
		/// </summary>
		public bool IsLocalClient { get { return Actor.LocalClient == this; } }

		public virtual float Health { get { return NativeMethods.Actor.GetPlayerHealth(this.GetActorHandle().Handle); } set { NativeMethods.Actor.SetPlayerHealth(this.GetActorHandle().Handle, value); } }
		public virtual float MaxHealth { get { return NativeMethods.Actor.GetPlayerMaxHealth(this.GetActorHandle().Handle); } set { NativeMethods.Actor.SetPlayerMaxHealth(this.GetActorHandle().Handle, value); } }

		public bool IsDead { get { return Health <= 0; } }

		internal HandleRef ActorHandleRef { get; set; }
		public int ChannelId { get; set; }
	}
}
