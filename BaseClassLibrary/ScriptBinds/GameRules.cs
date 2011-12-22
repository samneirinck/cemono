using System;
using System.Runtime.CompilerServices;

namespace CryEngine
{
	/// <summary>
	/// GameRules contains scriptbinds used in relation to the game rules system.
	/// </summary>
    public class GameRules
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern protected static void _RegisterGameMode(string gamemode);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern protected static void _AddGameModeAlias(string gamemode, string alias);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern protected static void _AddGameModeLevelLocation(string gamemode, string location);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern protected static void _SetDefaultGameMode(string gamemode);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern protected static UInt32 _SpawnPlayer(int channelId, string name, string className, Vec3 pos, Vec3 angles);

        /// <summary>
        /// Registers a gamemode.
        /// </summary>
        public static void RegisterGameMode(string gameMode)
        {
			_RegisterGameMode(gameMode);
        }

        /// <summary>
        /// Adds an alias name for the specified game rules (i.e. "vs" for the "Versus" gamemode.)
        /// </summary>
        public static void AddGameModeAlias(string gameMode, string alias)
        {
            _AddGameModeAlias(gameMode, alias);
        }

        /// <summary>
        /// Adds a default level location for the specified game rules. Level system will look up levels here.
        /// </summary>
        public static void AddGameModeLevelLocation(string gameMode, string location)
        {
            _AddGameModeLevelLocation(gameMode, location);
        }

        /// <summary>
        /// Sets the default game mode.
        /// </summary>
        public static void SetDefaultGameMode(string className)
        {
            _SetDefaultGameMode(className);
        }

        /// <summary>
        /// Creates the player actor.
        /// </summary>
        /// <param name="channelId"></param>
        /// <param name="name"></param>
        /// <param name="className"></param>
        /// <param name="pos"></param>
        /// <param name="angles"></param>
        public static Player SpawnPlayer(int channelId, string name, string className, Vec3 pos, Vec3 angles)
        {
            return new Player(_SpawnPlayer(channelId, name, className, pos, angles), channelId);
        }
    }
}