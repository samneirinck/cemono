using System;
using System.Collections.Generic;

using System.Linq;

using System.Runtime.CompilerServices;
using CryEngine.Native;

namespace CryEngine
{
	/// <summary>
	/// Represents a CryENGINE level.
	/// </summary>
	public class Level
	{
        #region Statics
		/// <summary>
		/// Loads a new level and returns its level info
		/// </summary>
		/// <param name="name"></param>
		/// <returns></returns>
		public static Level Load(string name)
		{
			Level level;
			if(Levels.TryGetValue(name, out level))
				return level;

			var ptr = NativeMethods.Level.LoadLevel(name);
			if(ptr != IntPtr.Zero)
				return new Level(ptr);

			return null;
		}

		/// <summary>
		/// Unloads the currently loaded level.
		/// </summary>
		public static void Unload()
		{
            NativeMethods.Level.UnloadLevel();
		}

		/// <summary>
		/// Gets the currently loaded level
		/// </summary>
		public static Level Current
		{
			get
			{
                var ptr = NativeMethods.Level.GetCurrentLevel();
				if(ptr != IntPtr.Zero)
				{
					if(Levels.Any(x => x.Value.LevelPointer == ptr))
					{
						// TODO
					}
					else
						return new Level(ptr);
				}

				return null;
			}
		}

		/// <summary>
		/// Returns true if any level is currently loaded
		/// </summary>
        public static bool Loaded { get { return NativeMethods.Level.IsLevelLoaded(); } }

		private static Dictionary<string, Level> Levels = new Dictionary<string, Level>();
		#endregion

		internal Level(IntPtr ptr)
		{
			LevelPointer = ptr;
		}

		/// <summary>
		/// Gets the supported game rules at the index; see SupportedGamerules.
		/// </summary>
		/// <param name="index"></param>
		/// <returns></returns>
		public string GetSupportedGameRules(int index)
		{
            return NativeMethods.Level.GetGameType(LevelPointer, index);
		}

		/// <summary>
		/// Returns true if this level supports the specific game rules.
		/// </summary>
		/// <param name="gamemodeName"></param>
		/// <returns></returns>
		public bool SupportsGameRules(string gamemodeName)
		{
            return NativeMethods.Level.SupportsGameType(LevelPointer, gamemodeName);
		}

		/// <summary>
		/// Gets the level name.
		/// </summary>
        public string Name { get { return NativeMethods.Level.GetName(LevelPointer); } }
		/// <summary>
		/// Gets the level display name.
		/// </summary>
        public string DisplayName { get { return NativeMethods.Level.GetDisplayName(LevelPointer); } }
		/// <summary>
		/// Gets the full path to the directory this level resides in.
		/// </summary>
        public string Path { get { return NativeMethods.Level.GetName(LevelPointer); } }

		/// <summary>
		/// Gets the heightmap size for this level.
		/// </summary>
        public int HeightmapSize { get { return NativeMethods.Level.GetHeightmapSize(LevelPointer); } }

		/// <summary>
		/// Gets the number of supported game rules for this level.
		/// </summary>
        public int SupportedGamerules { get { return NativeMethods.Level.GetGameTypeCount(LevelPointer); } }
		/// <summary>
		/// Gets the default gamemode for this level.
		/// </summary>
        public string DefaultGameRules { get { return NativeMethods.Level.GetDefaultGameType(LevelPointer); } }
		/// <summary>
		/// Returns true if this level is configured to support any gamemodes.
		/// </summary>
        public bool HasGameRules { get { return NativeMethods.Level.HasGameRules(LevelPointer); } }

		internal IntPtr LevelPointer { get; set; }
	}
}
