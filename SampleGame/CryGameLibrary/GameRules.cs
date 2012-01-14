using System;
using System.Runtime.CompilerServices;
using System.Diagnostics;

using System.Linq;
using System.Collections.Generic;

namespace CryEngine
{
    public partial class GameRules
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern protected static UInt32 _RevivePlayer(UInt32 playerId, Vec3 pos, Vec3 rot, int teamId, bool clearInventory);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern protected static UInt32 _SpawnPlayer(int channelId, string name, string className, Vec3 pos, Vec3 angles);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void _InitHits();
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static string _GetHitMaterialName(int materialId);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static string _GetHitType(int id);

        public static void RevivePlayer(UInt32 playerId, Vec3 pos, Vec3 rot, int teamId = 0, bool clearInventory = true)
        {
            _RevivePlayer(playerId, pos, rot, teamId, clearInventory);
        }

        /// <summary>
        /// Creates the player actor.
        /// </summary>
        /// <param name="channelId"></param>
        /// <param name="name"></param>
        /// <param name="pos"></param>
        /// <param name="angles"></param>
        public static T SpawnPlayer<T>(int channelId, string name, Vec3 pos, Vec3 angles) where T : BasePlayer, new()
        {
            if (m_players == null)
                m_players = new List<BasePlayer>();

            T player = new T();

            player.Initialize(_SpawnPlayer(channelId, name, "Player", pos, angles), channelId);

            m_players.Add(player);
            return player;
        }

        public static BasePlayer GetPlayer(uint playerId)
        {
            return m_players.Where(player => player.Id == playerId).FirstOrDefault();
        }

        public static T GetPlayer<T>(uint playerId) where T : BasePlayer
        {
            return GetPlayer(playerId) as T;
        }

        private static List<BasePlayer> m_players;
    }
}
