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
		extern internal static uint _RevivePlayer(UInt32 playerId, Vec3 pos, Vec3 rot, int teamId, bool clearInventory);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static uint _SpawnPlayer(int channelId, string name, string className, Vec3 pos, Vec3 angles);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static uint _GetPlayer();

		public static void RevivePlayer(EntityId playerId, Vec3 pos, Vec3 rot, int teamId = 0, bool clearInventory = true)
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
				Players = new List<BasePlayer>();
			else
			{
				var duplicatePlayers = Players.Where(player => player.ChannelId == channelId).ToArray();
				foreach (var player in duplicatePlayers)
				{
					EntitySystem.RemoveEntity(player.Id);

					Players.Remove(player);
				}

				duplicatePlayers = null;
			}

			uint entityId = _SpawnPlayer(channelId, name, "Player", pos, angles);
			if(entityId == 0)
			{
				Console.LogAlways("GameRules.SpawnPlayer failed; new entityId was invalid");
				return null;
			}

			int scriptId = ScriptCompiler.AddScriptInstance(new T());
			if(scriptId == -1)
			{
				Console.LogAlways("GameRules.SpawnPlayer failed; new scriptId was invalid");
				return null;
			}

			Players.Add(ScriptCompiler.GetScriptInstanceById(scriptId) as BasePlayer);
			Players.Last().InternalSpawn(entityId, channelId);

			return Players.Last() as T;
		}

		public static T GetLocalPlayer<T>() where T : BasePlayer
		{
			return GetPlayer<T>(_GetPlayer());
		}

		public static BasePlayer GetPlayer(EntityId playerId)
		{
			int scriptId = ScriptCompiler.GetEntityScriptId(playerId, typeof(BasePlayer));
			if(scriptId != -1)
				return ScriptCompiler.GetScriptInstanceById(scriptId) as BasePlayer;

			return null;
		}

		public static T GetPlayer<T>(EntityId playerId) where T : BasePlayer
		{
			return Players.Find(player => player.Id == playerId) as T;
		}

		public static List<BasePlayer> Players { get; private set; }
	}
}