using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;

using CryEngine.Initialization;
using CryEngine.Extensions;

namespace CryEngine
{
	internal interface INativeActorMethods
	{
		void RegisterClass(string className, bool isAI);
	}

	/// <summary>
	/// WIP Player class. TODO: Redo, currently very limited in terms of callbacks + interoperability with C++ backend
	/// </summary>
	public abstract class Actor : EntityBase
	{
		private static INativeActorMethods _actormethods;
		internal static INativeActorMethods Actormethods
		{
			get { return _actormethods ?? (_actormethods = new ActorMethods()); }
			set { _actormethods = value; }
		}

		class ActorMethods : INativeActorMethods
		{
			public void RegisterClass(string className, bool isAI)
			{
				Actor._RegisterActorClass(className, isAI);
			}
		}

		#region Externals
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _RegisterActorClass(string className, bool isAI);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static float _GetPlayerHealth(IntPtr actorPtr);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _SetPlayerHealth(IntPtr actorPtr, float newHealth);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static float _GetPlayerMaxHealth(IntPtr actorPtr);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _SetPlayerMaxHealth(IntPtr actorPtr, float newMaxHealth);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static ActorInfo _GetActorInfoByChannelId(ushort channelId);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static ActorInfo _GetActorInfoById(uint entId);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern private static ActorInfo _CreateActor(int channelId, string name, string className, Vec3 pos, Vec3 angles, Vec3 scale);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _RemoveActor(uint id);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static uint _GetClientActorId();
		#endregion

		#region Statics
		internal static void Load(CryScript script)
		{
			bool registerActorClass = true;
			bool isAI = false;

			ActorAttribute attr;
			if(script.Type.TryGetAttribute<ActorAttribute>(out attr))
			{
				registerActorClass = attr.useMonoActor;
				isAI = attr.isAI;
			}

			if(registerActorClass)
				Actormethods.RegisterClass(script.ScriptName, isAI);
		}

		public static Actor Get(int channelId)
		{
			var actor = Get<Actor>(channelId);
			if(actor != null)
				return actor;

			var entityInfo = _GetActorInfoByChannelId((ushort)channelId);
			if(entityInfo.Id != 0)
				return CreateNativeActor(entityInfo);

			return null;
		}

		public static T Get<T>(int channelId) where T : Actor
		{
			return ScriptManager.Find<T>(ScriptType.Actor, x => x.ChannelId == channelId);
		}

		public static Actor Get(EntityId actorId)
		{
			var actor = Get<Actor>(actorId);
			if(actor != null)
				return actor;

			// Couldn't find a CryMono entity, check if a non-managed one exists.
			var actorInfo = _GetActorInfoById(actorId);
			if(actorInfo.Id != 0)
				return CreateNativeActor(actorInfo);

			return null;
		}

		public static T Get<T>(EntityId actorId) where T : Actor
		{
			if(actorId == 0)
				throw new ArgumentException("actorId cannot be 0!");

			return ScriptManager.Find<T>(ScriptType.Actor, x => x.Id == actorId);
		}

		internal static Actor CreateNativeActor(ActorInfo actorInfo)
		{
			if(actorInfo.Id == 0)
				throw new ArgumentException("actorInfo.Id cannot be 0!");
			else if(actorInfo.ActorPtr == null)
				throw new ArgumentException("actorInfo.ActorPtr cannot be 0!");
			else if(actorInfo.EntityPtr == null)
				throw new ArgumentException("actorInfo.EntityPtr cannot be 0!");

			var nativeActor = new NativeActor(actorInfo);
			ScriptManager.AddScriptInstance(nativeActor);

			return nativeActor;
		}

		public static Actor Client 
		{
			get
			{
				var clientActorId = _GetClientActorId();
				if(clientActorId == 0)
					return null;

				return Get(new EntityId(clientActorId)); 
			} 
		}

		public static T Create<T>(int channelId, string name, string className,  Vec3 pos, Vec3 angles, Vec3 scale) where T : Actor, new()
		{
			// just in case
			Actor.Remove(channelId);

			var info = _CreateActor(channelId, name, className, pos, angles, scale);
			if(info.Id == 0)
			{
				Debug.LogAlways("[Actor.Create] New entityId was invalid");
				return null;
			}

			var player = new T();
			if(player == null)
			{
				Debug.LogAlways("[Actor.Create] Failed to add script instance");
				return null;
			}

			ScriptManager.AddScriptInstance(player, ScriptType.Actor);
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
			_RemoveActor(id);

			ScriptManager.RemoveInstances<Actor>(ScriptType.Actor, actor => actor.Id == id);
		}

		public static void Remove(Actor actor)
		{
			Remove(actor.Id);
		}

		public static void Remove(int channelId)
		{
			var actorInfo = _GetActorInfoByChannelId((ushort)channelId);
			if(actorInfo.Id != 0)
				_RemoveActor(actorInfo.Id);

			ScriptManager.RemoveInstances<Actor>(ScriptType.Actor, actor => actor.ChannelId == channelId);
		}
		#endregion

		/// <summary>
		/// Initializes the player.
		/// </summary>
		/// <param name="actorInfo"></param>
		/// <param name="channelId"></param>
		internal void InternalSpawn(ActorInfo actorInfo, int channelId)
		{
			Id = new EntityId(actorInfo.Id);
			ActorPointer = actorInfo.ActorPtr;
			EntityPointer = actorInfo.EntityPtr;

			ChannelId = channelId;

			OnSpawn();
		}

		public override void OnScriptReload()
		{
			base.OnScriptReload();

			ActorPointer = _GetActorInfoById(Id).ActorPtr;
		}

		internal IntPtr ActorPointer { get; set; }
		public int ChannelId { get; set; }

		public float Health { get { return _GetPlayerHealth(ActorPointer); } set { _SetPlayerHealth(ActorPointer, value); } }
		public float MaxHealth { get { return _GetPlayerMaxHealth(ActorPointer); } set { _SetPlayerMaxHealth(ActorPointer, value); } }

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
