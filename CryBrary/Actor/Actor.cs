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
#if !(RELEASE && RELEASE_DISABLE_CHECKS)
			if(actorId == 0)
				throw new ArgumentException("actorId cannot be 0!");
#endif

			return ScriptManager.Instance.Find<T>(ScriptType.Actor, x => x.Id == actorId);
		}

		internal static Actor CreateNativeActor(ActorInfo actorInfo)
		{
#if !(RELEASE && RELEASE_DISABLE_CHECKS)
			if(actorInfo.Id == 0)
				throw new ArgumentException("actorInfo.Id cannot be 0!");
			if(actorInfo.ActorPtr == IntPtr.Zero)
				throw new ArgumentException("actorInfo.ActorPtr cannot be 0!");
			if(actorInfo.EntityPtr == IntPtr.Zero)
				throw new ArgumentException("actorInfo.EntityPtr cannot be 0!");
#endif

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

		public static T Create<T>(int channelId, string name = "Dude", Vec3? pos = null, Vec3? angles = null, Vec3? scale = null, string className = null) where T : Actor, new()
		{
			// just in case
			Remove(channelId);

			var info = NativeMethods.Actor.CreateActor(channelId, name, className ?? typeof(T).Name, pos ?? new Vec3(0,0,0), angles ?? new Vec3(0,0,0), scale ?? new Vec3(1,1,1));
			if(info.Id == 0)
			{
				Debug.LogAlways("[Actor.Create] New entityId was invalid");
				return null;
			}

			var player = new T();
			ScriptManager.Instance.AddScriptInstance(player, ScriptType.Actor);
			player.InternalSpawn(info, channelId);

			// player must have physics
			player.Physics.Type = PhysicalizationType.Rigid;

			return player;
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
			ActorHandleRef = new HandleRef(this, actorInfo.ActorPtr);
			SetEntityHandle(actorInfo.EntityPtr);

			ChannelId = channelId;

			OnSpawn();
		}

		internal void SetActorHandle(IntPtr ptr)
		{
			ActorHandleRef = new HandleRef(this, ptr);
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
				hash = hash * 29 + ActorHandleRef.Handle.GetHashCode();
                hash = hash * 29 + base.EntityHandleRef.Handle.GetHashCode();

                return hash;
            }
        }

        internal override void OnScriptReloadInternal()
		{
            ActorHandleRef = new HandleRef(this, NativeMethods.Actor.GetActorInfoById(Id).ActorPtr);

            base.OnScriptReloadInternal();
		}
        #endregion

		public HandleRef ActorHandleRef { get; private set; }
		public int ChannelId { get; set; }

		public float Health { get { return NativeMethods.Actor.GetPlayerHealth(ActorHandleRef.Handle); } set { NativeMethods.Actor.SetPlayerHealth(ActorHandleRef.Handle, value); } }
		public float MaxHealth { get { return NativeMethods.Actor.GetPlayerMaxHealth(ActorHandleRef.Handle); } set { NativeMethods.Actor.SetPlayerMaxHealth(ActorHandleRef.Handle, value); } }

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
