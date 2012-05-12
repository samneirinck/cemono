using System;
using System.Runtime.CompilerServices;

using System.Collections.Generic;
using System.Linq;

using CryEngine.Initialization;
using CryEngine.Extensions;

namespace CryEngine
{
	/// <summary>
	/// WIP Player class. TODO: Redo, currently very limited in terms of callbacks + interoperability with C++ backend
	/// </summary>
    public abstract class Actor : EntityBase
	{
		#region Externals
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _RegisterActorClass(string className, bool isAI);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static float _GetPlayerHealth(uint playerId);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _SetPlayerHealth(uint playerId, float newHealth);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static float _GetPlayerMaxHealth(uint playerId);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _SetPlayerMaxHealth(uint playerId, float newMaxHealth);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static EntityId _GetEntityIdForChannelId(ushort channelId);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern private static EntityId _CreateActor(int channelId, string name, string className, Vec3 pos, Vec3 angles, Vec3 scale);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _RemoveActor(uint id);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static EntityId _GetClientActor();
		#endregion

		#region Statics
		public static Actor Get(int channelId)
		{
			var actor = Get<Actor>(channelId);
			if(actor != null)
				return actor;

			var entityId = _GetEntityIdForChannelId((ushort)channelId);
			if(entityId != 0)
				return CreateNativeActor(entityId);

			return null;
		}

		public static T Get<T>(int channelId) where T : Actor
		{
			return Get(x => x.ChannelId == channelId && x is T) as T;
		}

		public static Actor Get(EntityId actorId)
		{
			var actor = Get<Actor>(actorId);
			if(actor != null)
				return actor;

			// Couldn't find a CryMono entity, check if a non-managed one exists.
			if(_EntityExists(actorId))
				return CreateNativeActor(actorId);

			return null;
		}

		public static T Get<T>(EntityId actorId) where T : Actor
		{
			if(actorId == 0)
				throw new ArgumentException("actorId cannot be 0!");

			return Get(x => x.Id == actorId && x is T) as T;
		}

		public static Actor Get(Predicate<Actor> match)
		{
			Actor actor = null;
			for(int i = 0; i < ScriptManager.CompiledScripts.Count; i++)
			{
				var script = ScriptManager.CompiledScripts[i];
				if(script.Type.Implements(typeof(Actor)) && script.ScriptInstances != null)
				{
					actor = script.ScriptInstances.Find(x => match(x as Actor)) as Actor;
					if(actor != null)
						return actor;
				}
			}

			return null;
		}

		static Actor CreateNativeActor(EntityId actorId)
		{
			int scriptIndex;
			var script = ScriptManager.GetScriptByType(typeof(NativeActor), out scriptIndex);

			if(script.ScriptInstances == null)
				script.ScriptInstances = new List<CryScriptInstance>();

			script.ScriptInstances.Add(new NativeActor(actorId));

			ScriptManager.CompiledScripts[scriptIndex] = script;

			return script.ScriptInstances.Last() as Actor;
		}

		public static Actor LocalPlayer { get { return Get(_GetClientActor()); } }

		public static T Create<T>(int channelId, string name, Vec3 pos, Vec3 angles, Vec3 scale) where T : Actor, new()
		{
			// just in case
			Actor.Remove(channelId);

			EntityId entityId = _CreateActor(channelId, name, "Player", pos, angles, scale);
			if(entityId == 0)
			{
				Debug.LogAlways("[Actor.Create] New entityId was invalid");
				return null;
			}

			var player = Initialization.ScriptManager.AddScriptInstance(new T()) as T;
			if(player == null)
			{
				Debug.LogAlways("[Actor.Create] Failed to add script instance");
				return null;
			}

			player.InternalSpawn(entityId, channelId);

			return player;
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

			InternalRemove(id);
		}

		public static void Remove(Actor actor)
		{
			Remove(actor.Id);
		}

		public static void Remove(int channelId)
		{
			Remove(_GetEntityIdForChannelId((ushort)channelId));
		}

		internal static void InternalRemove(EntityId id)
		{
			foreach(var script in ScriptManager.CompiledScripts)
			{
				if(script.ScriptInstances != null)
					script.ScriptInstances.RemoveAll(instance => instance is Actor && (instance as Actor).Id == id);
			}
		}
		#endregion

        /// <summary>
        /// Initializes the player.
        /// </summary>
        /// <param name="entityId"></param>
        /// <param name="channelId"></param>
		internal void InternalSpawn(EntityId entityId, int channelId)
        {
			Id = entityId;
			ChannelId = channelId;

			InitPhysics();

			OnSpawn();
        }

		#region Callbacks
		public void OnSpawn() { }
		#endregion

		public int ChannelId { get; set; }

		public float Health { get { return _GetPlayerHealth(Id); } set { _SetPlayerHealth(Id, value); } }
		public float MaxHealth { get { return _GetPlayerMaxHealth(Id); } set { _SetPlayerMaxHealth(Id, value); } }

        public bool IsDead() { return Health <= 0; }
    }
}
