using System;
using System.IO;

using CryEngine.Initialization;

namespace CryEngine.Utilities
{
    /// <summary>
    /// Provides a clean method of retrieving commonly used engine filepaths.
    /// </summary>
    public static class PathUtils
    {
        /// <summary>
        /// Gets the root engine folder.
        /// </summary>
        /// <returns>The filepath to the root folder as a string, ex:</returns>
        /// <example>"C:\CryENGINE3"</example>
        static string RootFolder { get { return Directory.GetCurrentDirectory(); } }

        /// <summary>
        /// Gets the folder containing binaries relevant to current build type (Bin32 / Bin64).
        /// </summary>
        /// <returns>The filepath to the Binary folder as a string, ex:
        /// <example>"C:\CryENGINE3\Bin32"</example></returns>
        public static string BinaryFolder { get { return Path.Combine(RootFolder, "Bin" + (IntPtr.Size * 8).ToString()); } }

        /// <summary>
        /// Gets the folder containing CryMono configuration files.
        /// </summary>
        public static string ConfigFolder { get; internal set; }

        /// <summary>
        /// Directory from which CryMono will load managed plugins.
        /// </summary>
        public static string PluginsFolder { get { return Path.Combine(ConfigFolder, "Plugins"); } }

        /// <summary>
        /// Gets the Mono folder containing essential Mono libraries and configuration files.
        /// </summary>
        public static string MonoFolder { get { return Path.Combine(ConfigFolder, "Mono"); } }

        /// <summary>
        /// Gets the engine folder.
        /// CryENGINE uses this store engine content which is not specific to the game being developed, i.e. shaders and configuration files.
        /// </summary>
        /// <returns>The filepath to the Engine folder as a string, ex:
        /// <example>"C:\CryENGINE3\Engine"</example></returns>
        public static string EngineFolder { get { return Path.Combine(RootFolder, "Engine"); } }

        /// <summary>
        /// Gets the current game folder.
        /// CryENGINE uses this as the root for all game-related content.
        /// </summary>
        /// <returns>The filepath to the Game folder as a string, ex:
        /// <example>"C:\CryENGINE3\Game"</example></returns>
        public static string GameFolder
        {
            get 
            {
                CVar cvar;
                if (CVar.TryGet("sys_game_folder", out cvar))
                    return Path.Combine(RootFolder, cvar.String);

                return Path.Combine(RootFolder, "Game");
            }
        }

        /// <summary>
        /// Gets the folder in which the engine stores scripts.
        /// We use this as the root for our .cs files and .dll plugins.
        /// </summary>
        /// <returns>The filepath to the Scripts folder as a string, ex:
        /// <example>"C:\CryENGINE3\Game\Scripts"</example></returns>
        public static string ScriptsFolder { get { return Path.Combine(GameFolder, "Scripts"); } }

		/// <summary>
		/// Directory from which CryMono loads scripts, e.g. *.cs files.
		/// </summary>
		public static string CryMonoScriptsFolder 
		{
			get 
			{
				CVar cvar;
				if (CVar.TryGet("mono_scriptDirectory", out cvar))
				{
					var alternateScriptsDir = cvar.String;
					if (!string.IsNullOrEmpty(alternateScriptsDir))
						return alternateScriptsDir;
				}

				return ScriptsFolder; 
			}
		}

        /// <summary>
        /// Gets the directory CryMono uses to store temporary files.
        /// </summary>
        public static string TempFolder { get { return Path.Combine(Path.GetTempPath(), "CryMono"); } }
    }
}