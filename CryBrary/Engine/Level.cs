using System;
using System.Collections.Generic;

using System.Linq;

using System.Runtime.CompilerServices;
using CryEngine.Native;

namespace CryEngine
{
	public class Level
	{
        private static INativeLevelMethods _nativeLevelMethods;
        internal static INativeLevelMethods NativeLevelMethods
        {
            get { return _nativeLevelMethods ?? (_nativeLevelMethods = new NativeLevelMethods()); }
            set { _nativeLevelMethods = value; }
        }

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

			var ptr = NativeLevelMethods.LoadLevel(name);
			if(ptr != IntPtr.Zero)
				return new Level(ptr);

			return null;
		}

		/// <summary>
		/// Unloads the currently loaded level.
		/// </summary>
		public static void Unload()
		{
			NativeLevelMethods.UnloadLevel();
		}

		/// <summary>
		/// Gets the currently loaded level
		/// </summary>
		public static Level Current
		{
			get
			{
				var ptr = NativeLevelMethods.GetCurrentLevel();
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
		public static bool Loaded { get { return NativeLevelMethods.IsLevelLoaded(); } }

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
			return NativeLevelMethods.GetGameType(LevelPointer, index);
		}

		/// <summary>
		/// Returns true if this level supports the specific game rules.
		/// </summary>
		/// <param name="gamemodeName"></param>
		/// <returns></returns>
		public bool SupportsGameRules(string gamemodeName)
		{
			return NativeLevelMethods.SupportsGameType(LevelPointer, gamemodeName);
		}

		/// <summary>
		/// Gets the level name.
		/// </summary>
		public string Name { get { return NativeLevelMethods.GetName(LevelPointer); } }
		/// <summary>
		/// Gets the level display name.
		/// </summary>
		public string DisplayName { get { return NativeLevelMethods.GetDisplayName(LevelPointer); } }
		/// <summary>
		/// Gets the full path to the directory this level resides in.
		/// </summary>
		public string Path { get { return NativeLevelMethods.GetName(LevelPointer); } }
		public string Paks { get { return NativeLevelMethods.GetName(LevelPointer); } }

		/// <summary>
		/// Gets the heightmap size for this level.
		/// </summary>
		public int HeightmapSize { get { return NativeLevelMethods.GetHeightmapSize(LevelPointer); } }

		/// <summary>
		/// Gets the number of supported game rules for this level.
		/// </summary>
		public int SupportedGamerules { get { return NativeLevelMethods.GetGameTypeCount(LevelPointer); } }
		/// <summary>
		/// Gets the default gamemode for this level.
		/// </summary>
		public string DefaultGameRules { get { return NativeLevelMethods.GetDefaultGameType(LevelPointer); } }
		/// <summary>
		/// Returns true if this level is configured to support any gamemodes.
		/// </summary>
		public bool HasGameRules { get { return NativeLevelMethods.HasGameRules(LevelPointer); } }

		internal IntPtr LevelPointer { get; set; }
	}
}
