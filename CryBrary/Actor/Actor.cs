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
	public abstract class Actor : ActorBase
	{
		#region Statics
		/// <summary>
		/// Gets the actor with the specified channel id if it exists.
		/// </summary>
		/// <param name="channelId"></param>
		/// <returns></returns>
		public static ActorBase Get(int channelId)
		{
			var actor = Get<ActorBase>(channelId);
			if(actor != null)
				return actor;

			var entityInfo = NativeMethods.Actor.GetActorInfoByChannelId((ushort)channelId);
			if(entityInfo.Id != 0)
				return CreateNativeActor(entityInfo);

			return null;
		}

		/// <summary>
		/// Gets the actor with the specified channel id if it exists.
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="channelId"></param>
		/// <returns></returns>
		public static T Get<T>(int channelId) where T : ActorBase
		{
			return ScriptManager.Instance.Find<T>(ScriptType.Actor, x => x.ChannelId == channelId);
		}


		/// <summary>
		/// Gets the actor with the specified entity id if it exists.
		/// </summary>
		/// <param name="actorId"></param>
		/// <returns></returns>
		public static ActorBase Get(EntityId actorId)
		{
			var actor = Get<ActorBase>(actorId);
			if(actor != null)
				return actor;

			// Couldn't find a CryMono entity, check if a non-managed one exists.
            var actorInfo = NativeMethods.Actor.GetActorInfoById(actorId);
			if(actorInfo.Id != 0)
				return CreateNativeActor(actorInfo);

			return null;
		}

		/// <summary>
		/// Gets the actor with the specified entity id if it exists.
		/// </summary>
		/// <param name="actorId"></param>
		/// <returns></returns>
		public static T Get<T>(EntityId actorId) where T : ActorBase
		{
#if !(RELEASE && RELEASE_DISABLE_CHECKS)
			if(actorId == 0)
				throw new ArgumentException("actorId cannot be 0!");
#endif

			return ScriptManager.Instance.Find<T>(ScriptType.Actor, x => x.Id == actorId);
		}

		internal static ActorBase CreateNativeActor(ActorInfo actorInfo)
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

		/// <summary>
		/// Gets the player actor in use on this PC.
		/// </summary>
		public static ActorBase LocalClient 
		{
			get
			{
                var clientActorId = NativeMethods.Actor.GetClientActorId();
				if(clientActorId == 0)
					return null;

				return Get(new EntityId(clientActorId)); 
			} 
		}

		/// <summary>
		/// Spawns a new actor
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="channelId"></param>
		/// <param name="name"></param>
		/// <param name="pos"></param>
		/// <param name="rot"></param>
		/// <param name="scale"></param>
		/// <returns></returns>
		public static T Create<T>(int channelId, string name = "Dude", Vec3? pos = null, Quat? rot = null, Vec3? scale = null) where T : ActorBase, new()
		{
			return Create(typeof(T), channelId, name, pos, rot, scale) as T;
		}
		
		/// <summary>
		/// Spawns a new actor
		/// </summary>
		/// <param name="actorType"></param>
		/// <param name="channelId"></param>
		/// <param name="name"></param>
		/// <param name="pos"></param>
		/// <param name="rot"></param>
		/// <param name="scale"></param>
		/// <returns></returns>
		public static ActorBase Create(Type actorType, int channelId, string name = "Dude", Vec3? pos = null, Quat? rot = null, Vec3? scale = null)
		{
			bool isNative = actorType.Implements(typeof(NativeActor));

			string className;
			if (isNative)
				className = actorType.Name;
			else
				className = "MonoActor";

			var actor = Get(channelId);
			if (actor != null)
				return actor;

			actor = Activator.CreateInstance(actorType) as ActorBase;

			var info = NativeMethods.Actor.CreateActor(actor as Actor, channelId, name, className, pos ?? new Vec3(0, 0, 0), rot ?? Quat.Identity, scale ?? new Vec3(1, 1, 1));
			if(info.Id == 0)
			{
				if (isNative)
					throw new Exception("Actor creation failed, make sure your IActor implementation is registered with the same name as your managed actor class.");
				else
					throw new Exception("Actor creation failed");
			}

			ScriptManager.Instance.AddScriptInstance(actor, ScriptType.Actor);
			actor.InternalSpawn(info);

			// actor must have physics
			actor.Physics.Type = PhysicalizationType.Rigid;

			return actor;
		}

		/// <summary>
		/// Spawns a new actor
		/// </summary>
		/// <param name="className"></param>
		/// <param name="channelId"></param>
		/// <param name="name"></param>
		/// <param name="pos"></param>
		/// <param name="rot"></param>
		/// <param name="scale"></param>
		/// <returns></returns>
		public static ActorBase Create(string className, int channelId, string name = "Dude", Vec3? pos = null, Quat? rot = null, Vec3? scale = null)
		{
			var actor = Get(channelId);
			if (actor != null)
				return actor;

			actor = new NativeActor();

			var info = NativeMethods.Actor.CreateActor(actor as Actor, channelId, name, className, pos ?? new Vec3(0, 0, 0), rot ?? Quat.Identity, scale ?? new Vec3(1, 1, 1));
			if (info.Id == 0)
				throw new Exception("Actor creation failed, make sure your IActor implementation is registered with the same name as your managed actor class.");

			ScriptManager.Instance.AddScriptInstance(actor, ScriptType.Actor);
			actor.InternalSpawn(info);

			return actor;
		}

		/// <summary>
		/// Removes an actor by entity id.
		/// </summary>
		/// <param name="id"></param>
		public static void Remove(EntityId id)
		{
            NativeMethods.Actor.RemoveActor(id);
		}

		/// <summary>
		/// Removes an actor by channelId
		/// </summary>
		/// <param name="channelId"></param>
		public static void Remove(int channelId)
		{
            var actorInfo = NativeMethods.Actor.GetActorInfoByChannelId((ushort)channelId);
			if(actorInfo.Id != 0)
                NativeMethods.Actor.RemoveActor(actorInfo.Id);
		}
		#endregion

		#region Callbacks
		/// <summary>
		/// Called when resetting the state of the entity in Editor.
		/// </summary>
		/// <param name="enteringGame">true if currently entering gamemode, false if exiting.</param>
		protected virtual void OnEditorReset(bool enteringGame) { }

		/// <summary>
		/// Called to update the view associated to this actor.
		/// </summary>
		/// <param name="viewParams"></param>
		protected virtual void UpdateView(ref ViewParams viewParams) { }

		/// <summary>
		/// Called prior to updating physics, useful for requesting movement.
		/// </summary>
		protected virtual void OnPrePhysicsUpdate() { }
		#endregion

		/// <summary>
		/// Sets / gets the current health of this actor.
		/// </summary>
		public override float Health { get; set; }
		/// <summary>
		/// Sets / gets the max health value for this actor.
		/// </summary>
		public override float MaxHealth { get; set; }
	}

	internal struct ActorInfo
	{
		public IntPtr EntityPtr;
		public IntPtr ActorPtr;
		public uint Id;
		public int ChannelId;
	}
}
