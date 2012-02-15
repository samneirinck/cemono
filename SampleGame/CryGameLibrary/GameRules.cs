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
			if (Players == null)
				Players = new Dictionary<uint, int>();

			uint entityId = _SpawnPlayer(channelId, name, "Player", pos, angles);
			if (entityId == 0)
				return null;

			int scriptId = ScriptCompiler.AddScriptInstance(new T());
			if (scriptId == -1)
				return null;

			Players.Add(entityId, scriptId);

			T player = ScriptCompiler.GetScriptInstanceById(scriptId) as T;
			player.InternalSpawn(entityId, channelId);

			return player;
        }

        public static BasePlayer GetPlayer(uint playerId)
        {
			if (Players.ContainsKey(playerId))
				return ScriptCompiler.GetScriptInstanceById(Players[playerId]) as BasePlayer;

			return null;
        }

        public static T GetPlayer<T>(uint playerId) where T : BasePlayer
        {
            return GetPlayer(playerId) as T;
        }

		public static Dictionary<uint /* entity id*/, int> Players;
    }
}
