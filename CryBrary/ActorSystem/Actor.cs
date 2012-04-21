using System;
using System.Runtime.CompilerServices;

namespace CryEngine
{
	/// <summary>
	/// WIP Player class. TODO: Redo, currently very limited in terms of callbacks + interoperability with C++ backend
	/// </summary>
    public abstract class Actor : Entity
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
		extern internal static void _RemoveActor(uint id);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static EntityId _GetClientActor();
		#endregion

		#region Statics
		public static EntityId GetEntityIdByChannelId(int channelId)
		{
			return new EntityId(_GetEntityIdForChannelId((ushort)channelId));
		}

		public static new Actor Get(EntityId actorId)
		{
			return Entity.Get(actorId) as Actor;
		}

		public static T Get<T>(EntityId actorId) where T : Actor
		{
			return Get(actorId) as T;
		}

		public static Actor LocalPlayer { get { return Get(_GetClientActor()); } }

		public static new void Remove(EntityId id)
		{
			_RemoveActor(id);

			Entity.RemoveInternalEntity(id);
		}

		public static void Remove(Actor actor)
		{
			Remove(actor.Id);
		}

		public static void Remove(int channelId)
		{
			Remove(GetEntityIdByChannelId(channelId));
		}
		#endregion

        /// <summary>
        /// Initializes the player.
        /// </summary>
        /// <param name="entityId"></param>
        /// <param name="channelId"></param>
		public void InternalSpawn(EntityId entityId, int channelId)
        {
			ChannelId = channelId;

			// Should be called second last, prior to OnSpawn
			SpawnCommon(entityId);

			OnSpawn();
        }

		#region Obsolete methods
		// TODO: Rework Actor class to implement these callbacks (and / or don't derive from Entity)

		[Obsolete("Not supported in the Actor class")]
		public override void OnSpawn() { }
		[Obsolete("Not supported in the Actor class")]
		protected override bool OnRemove() { return true; }
		[Obsolete("Not supported in the Actor class")]
		protected override void OnReset(bool enteringGame) { }
		[Obsolete("Not supported in the Actor class")]
		protected override void OnStartGame() { }
		[Obsolete("Not supported in the Actor class")]
		protected override void OnStartLevel() { }
		[Obsolete("Not supported in the Actor class")]
		protected override void OnEnterArea(EntityId triggerEntityId, EntityId areaEntityId) { }
		[Obsolete("Not supported in the Actor class")]
		protected override void OnLeaveArea(EntityId triggerEntityId, EntityId areaEntityId) { }
		[Obsolete("Not supported in the Actor class")]
		protected override void OnCollision(EntityId targetEntityId, Vec3 hitPos, Vec3 dir, short materialId, Vec3 contactNormal) { }
		[Obsolete("Not supported in the Actor class")]
		public override void OnHit(HitInfo hitInfo) { }
		#endregion

		public int ChannelId { get; set; }
		public float Health { get { return _GetPlayerHealth(Id); } set { _SetPlayerHealth(Id, value); } }
		public float MaxHealth { get { return _GetPlayerMaxHealth(Id); } set { _SetPlayerMaxHealth(Id, value); } }

        public bool IsDead() { return Health <= 0; }

		internal override bool CanContainEditorProperties { get { return false; } }
    }
}
