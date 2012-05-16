using System;
using System.Collections.Generic;

using System.Linq;

using System.Runtime.CompilerServices;

namespace CryEngine
{
	public class Level
	{
		#region Externals
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static IntPtr _GetCurrentLevel();
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static IntPtr _LoadLevel(string name);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static bool _IsLevelLoaded();
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _UnloadLevel();

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static string _GetName(IntPtr levelPtr);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static string _GetPath(IntPtr levelPtr);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static string _GetPaks(IntPtr levelPtr);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static string _GetDisplayName(IntPtr levelPtr);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static int _GetHeightmapSize(IntPtr levelPtr);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static int _GetGameTypeCount(IntPtr levelPtr);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static string _GetGameType(IntPtr levelPtr, int index);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static bool _SupportsGameType(IntPtr levelPtr, string gameTypeName);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static string _GetDefaultGameType(IntPtr levelPtr);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static bool _HasGameRules(IntPtr levelPtr);
		#endregion

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

			var ptr = _LoadLevel(name);
			if(ptr != null)
				return new Level(ptr);

			return null;
		}

		/// <summary>
		/// Unloads the currently loaded level.
		/// </summary>
		public static void Unload()
		{
			_UnloadLevel();
		}

		/// <summary>
		/// Gets the currently loaded level
		/// </summary>
		public static Level Current
		{
			get
			{
				var ptr = _GetCurrentLevel();
				if(ptr != null)
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
		public static bool Loaded { get { return _IsLevelLoaded(); } }

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
			return _GetGameType(LevelPointer, index);
		}

		/// <summary>
		/// Returns true if this level supports the specific game rules.
		/// </summary>
		/// <param name="gamemodeName"></param>
		/// <returns></returns>
		public bool SupportsGameRules(string gamemodeName)
		{
			return _SupportsGameType(LevelPointer, gamemodeName);
		}

		/// <summary>
		/// Gets the level name.
		/// </summary>
		public string Name { get { return _GetName(LevelPointer); } }
		/// <summary>
		/// Gets the level display name.
		/// </summary>
		public string DisplayName { get { return _GetDisplayName(LevelPointer); } }
		/// <summary>
		/// Gets the full path to the directory this level resides in.
		/// </summary>
		public string Path { get { return _GetName(LevelPointer); } }
		public string Paks { get { return _GetName(LevelPointer); } }

		/// <summary>
		/// Gets the heightmap size for this level.
		/// </summary>
		public int HeightmapSize { get { return _GetHeightmapSize(LevelPointer); } }

		/// <summary>
		/// Gets the number of supported game rules for this level.
		/// </summary>
		public int SupportedGamerules { get { return _GetGameTypeCount(LevelPointer); } }
		/// <summary>
		/// Gets the default gamemode for this level.
		/// </summary>
		public string DefaultGameRules { get { return _GetDefaultGameType(LevelPointer); } }
		/// <summary>
		/// Returns true if this level is configured to support any gamemodes.
		/// </summary>
		public bool HasGameRules { get { return _HasGameRules(LevelPointer); } }

		internal IntPtr LevelPointer { get; set; }
	}
}
