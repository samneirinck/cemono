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

		public static T Get<T>(int channelId) where T : ActorBase
		{
			return ScriptManager.Instance.Find<T>(ScriptType.Actor, x => x.ChannelId == channelId);
		}

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

		public static T Create<T>(int channelId, string name = "Dude", Vec3? pos = null, Vec3? angles = null, Vec3? scale = null) where T : ActorBase, new()
		{
			return Create(typeof(T), channelId, name, pos, angles, scale) as T;
		}
		
		public static ActorBase Create(Type actorType, int channelId, string name = " Dude", Vec3? pos = null, Vec3? angles = null, Vec3? scale = null)
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

			var info = NativeMethods.Actor.CreateActor(actor as Actor, channelId, name, className, pos ?? new Vec3(0, 0, 0), angles ?? new Vec3(0, 0, 0), scale ?? new Vec3(1, 1, 1));
			if(info.Id == 0)
			{
				if (isNative)
					throw new Exception("Actor creation failed, make sure your IActor implementation is registered with the same name as your managed actor class.");
				else
					throw new Exception("Actor creation failed");
			}

			ScriptManager.Instance.AddScriptInstance(actor, ScriptType.Actor);
			actor.InternalSpawn(info, channelId);

			// actor must have physics
			actor.Physics.Type = PhysicalizationType.Rigid;

			return actor;
		}

		public static void Remove(EntityId id)
		{
            NativeMethods.Actor.RemoveActor(id);

			ScriptManager.Instance.RemoveInstances<ActorBase>(ScriptType.Actor, actor => actor.Id == id);
		}

		public static void Remove(int channelId)
		{
            var actorInfo = NativeMethods.Actor.GetActorInfoByChannelId((ushort)channelId);
			if(actorInfo.Id != 0)
                NativeMethods.Actor.RemoveActor(actorInfo.Id);

			ScriptManager.Instance.RemoveInstances<ActorBase>(ScriptType.Actor, actor => actor.ChannelId == channelId);
		}
		#endregion

		#region Callbacks
		/// <summary>
		/// Called when resetting the state of the entity in Editor.
		/// </summary>
		/// <param name="enteringGame">true if currently entering gamemode, false if exiting.</param>
		protected virtual void OnEditorReset(bool enteringGame) { }

		protected virtual void UpdateView(ref ViewParams viewParams) { }

		protected virtual void OnPrePhysicsUpdate() { }
		#endregion

		public override float Health { get; set; }
		public override float MaxHealth { get; set; }
	}

	internal struct ActorInfo
	{
		public IntPtr EntityPtr;
		public IntPtr ActorPtr;
		public uint Id;
	}
}
