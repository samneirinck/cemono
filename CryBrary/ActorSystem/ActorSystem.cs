using System.Runtime.CompilerServices;

namespace CryEngine
{
    /// <summary>
    /// Used internally by this assembly for the Player base class.
    /// </summary>
    public static class ActorSystem
    {
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
		extern internal static uint _GetEntityIdForChannelId(ushort channelId);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _RemoveActor(uint id);

		public static EntityId GetEntityIdForChannelId(int channelId)
		{
			return new EntityId(_GetEntityIdForChannelId((ushort)channelId));
		}

		public static void RemoveActor(EntityId id)
		{
			_RemoveActor(id);

			Entity.RemoveInternalEntity(id);
		}

		public static void RemoveActor(int channelId)
		{
			RemoveActor(GetEntityIdForChannelId(channelId));
		}
    }
}
