using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using CryEngine.Initialization;
using CryEngine.Extensions;
using CryEngine.Native;

namespace CryEngine
{
    /// <summary>
	/// WIP Player class. TODO: Redo, currently very limited in terms of callbacks + interoperability with C++ backend
	/// </summary>
	public abstract class Actor : EntityBase
	{
		#region Statics
		public static Actor Get(int channelId)
		{
			var actor = Get<Actor>(channelId);
			if(actor != null)
				return actor;

			var entityInfo = NativeMethods.Actor.GetActorInfoByChannelId((ushort)channelId);
			if(entityInfo.Id != 0)
				return CreateNativeActor(entityInfo);

			return null;
		}

		public static T Get<T>(int channelId) where T : Actor
		{
			return ScriptManager.Instance.Find<T>(ScriptType.Actor, x => x.ChannelId == channelId);
		}

		public static Actor Get(EntityId actorId)
		{
			var actor = Get<Actor>(actorId);
			if(actor != null)
				return actor;

			// Couldn't find a CryMono entity, check if a non-managed one exists.
            var actorInfo = NativeMethods.Actor.GetActorInfoById(actorId);
			if(actorInfo.Id != 0)
				return CreateNativeActor(actorInfo);

			return null;
		}

		public static T Get<T>(EntityId actorId) where T : Actor
		{
			if(actorId == 0)
				throw new ArgumentException("actorId cannot be 0!");

			return ScriptManager.Instance.Find<T>(ScriptType.Actor, x => x.Id == actorId);
		}

		internal static Actor CreateNativeActor(ActorInfo actorInfo)
		{
			if(actorInfo.Id == 0)
				throw new ArgumentException("actorInfo.Id cannot be 0!");
			if(actorInfo.ActorPtr == IntPtr.Zero)
				throw new ArgumentException("actorInfo.ActorPtr cannot be 0!");
			if(actorInfo.EntityPtr == IntPtr.Zero)
				throw new ArgumentException("actorInfo.EntityPtr cannot be 0!");

			var nativeActor = new NativeActor(actorInfo);
			ScriptManager.Instance.AddScriptInstance(nativeActor, ScriptType.Actor);

			return nativeActor;
		}

		public static Actor Client 
		{
			get
			{
                var clientActorId = NativeMethods.Actor.GetClientActorId();
				if(clientActorId == 0)
					return null;

				return Get(new EntityId(clientActorId)); 
			} 
		}

		public static T Create<T>(int channelId, string name, string className,  Vec3 pos, Vec3 angles, Vec3 scale) where T : Actor, new()
		{
			// just in case
			Remove(channelId);

            var info = NativeMethods.Actor.CreateActor(channelId, name, className, pos, angles, scale);
			if(info.Id == 0)
			{
				Debug.LogAlways("[Actor.Create] New entityId was invalid");
				return null;
			}

			var player = new T();
			ScriptManager.Instance.AddScriptInstance(player, ScriptType.Actor);
			player.InternalSpawn(info, channelId);

			return player;
		}

		public static T Create<T>(int channelId, string name, Vec3 pos, Vec3 angles, Vec3 scale) where T : Actor, new()
		{
			return Create<T>(channelId, name, typeof(T).Name, pos, angles, scale);
		}

		public static T Create<T>(int channelId, string name, Vec3 pos, Vec3 angles) where T : Actor, new()
		{
			return Create<T>(channelId, name, pos, angles, new Vec3(1, 1, 1));
		}

		public static T Create<T>(int channelId, string name, Vec3 pos) where T : Actor, new()
		{
			return Create<T>(channelId, name, pos, Vec3.Zero, new Vec3(1, 1, 1));
		}

		public static T Create<T>(int channelId, string name) where T : Actor, new()
		{
			return Create<T>(channelId, name, Vec3.Zero, Vec3.Zero, new Vec3(1, 1, 1));
		}

		public static new void Remove(EntityId id)
		{
            NativeMethods.Actor.RemoveActor(id);

			ScriptManager.Instance.RemoveInstances<Actor>(ScriptType.Actor, actor => actor.Id == id);
		}

		public static void Remove(Actor actor)
		{
			Remove(actor.Id);
		}

		public static void Remove(int channelId)
		{
            var actorInfo = NativeMethods.Actor.GetActorInfoByChannelId((ushort)channelId);
			if(actorInfo.Id != 0)
                NativeMethods.Actor.RemoveActor(actorInfo.Id);

			ScriptManager.Instance.RemoveInstances<Actor>(ScriptType.Actor, actor => actor.ChannelId == channelId);
		}
		#endregion

		/// <summary>
		/// Initializes the player.
		/// </summary>
		/// <param name="actorInfo"></param>
		/// <param name="channelId"></param>
		internal void InternalSpawn(ActorInfo actorInfo, int channelId)
		{
            System.Diagnostics.Contracts.Contract.Requires(channelId > 0);
			Id = new EntityId(actorInfo.Id);
			HandleRef = new HandleRef(this, actorInfo.ActorPtr);
			base.HandleRef = new HandleRef(this, actorInfo.EntityPtr);

			ChannelId = channelId;

			OnSpawn();
		}

        #region Overrides
        public override int GetHashCode()
        {
            unchecked // Overflow is fine, just wrap
            {
                int hash = 17;

                hash = hash * 29 + ScriptId.GetHashCode();
                hash = hash * 29 + Id.GetHashCode();
                hash = hash * 29 + ChannelId.GetHashCode();
				hash = hash * 29 + HandleRef.GetHashCode();
                hash = hash * 29 + base.HandleRef.GetHashCode();

                return hash;
            }
        }

        internal override void OnScriptReloadInternal()
		{
            HandleRef = new HandleRef(this, NativeMethods.Actor.GetActorInfoById(Id).ActorPtr);

            base.OnScriptReloadInternal();
		}
        #endregion

		public new HandleRef HandleRef { get; set; }
		public int ChannelId { get; set; }

		public float Health { get { return NativeMethods.Actor.GetPlayerHealth(HandleRef.Handle); } set { NativeMethods.Actor.SetPlayerHealth(HandleRef.Handle, value); } }
		public float MaxHealth { get { return NativeMethods.Actor.GetPlayerMaxHealth(HandleRef.Handle); } set { NativeMethods.Actor.SetPlayerMaxHealth(HandleRef.Handle, value); } }

		public bool IsDead() { return Health <= 0; }
	}

    [AttributeUsage(AttributeTargets.Class)]
	public sealed class ActorAttribute : Attribute
	{
		public ActorAttribute(bool useMonoActor = true, bool isAI = false)
		{
			this.useMonoActor = useMonoActor;
			this.isAI = isAI;
		}

		/// <summary>
		/// Utilize the C++ Actor class contained within CryMono.dll
		/// Otherwise the engine will require one created in the game dll. 
		/// </summary>
		public bool useMonoActor;

		/// <summary>
		/// Determines if this is an AI actor class.
		/// Only applied when UseMonoActor is set to true.
		/// </summary>
		public bool isAI;
	}

	internal struct ActorInfo
	{
		public IntPtr EntityPtr;
		public IntPtr ActorPtr;
		public uint Id;
	}
}
