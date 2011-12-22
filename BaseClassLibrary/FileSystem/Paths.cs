using System;
using System.IO;

namespace CryEngine
{
	/// <summary>
	/// CryPath provides a clean method of retrieving commonly used engine filepaths.
	/// </summary>
    public static class CryPath
    {
        /// <summary>
        /// Gets the current folder containing binaries relevant to current build type (Bin32 / Bin64).
        /// </summary>
        /// <returns>The filepath to the Binary folder as a string, ex:
        /// <example>"C:\CryENGINE3\Bin32\"</example></returns>
        public static string GetBinaryFolder()
        {
            return Path.Combine(Directory.GetCurrentDirectory(), "Bin" + (IntPtr.Size * 8).ToString());
        }

        /// <summary>
        /// Gets the engine folder.
        /// CryENGINE uses this store engine content which is not specific to the game being developed, i.e. shaders and configuration files.
        /// </summary>
        /// <returns>The filepath to the Engine folder as a string, ex:
        /// <example>"C:\CryENGINE3\Engine\"</example></returns>
        public static string GetEngineFolder()
        {
            return Path.Combine(Directory.GetCurrentDirectory(), "Engine");
        }

        /// <summary>
        /// Gets the current game folder.
        /// CryENGINE uses this as the root for all game-related content.
        /// </summary>
        /// <returns>The filepath to the Game folder as a string, ex:
        /// <example>"C:\CryENGINE3\Game\"</example></returns>
        public static string GetGameFolder()
        {
            return Path.Combine(Directory.GetCurrentDirectory(), "Game"/*CryConsole.GetCVar("sys_game_folder").String*/);
        }

        /// <summary>
        /// Gets the folder in which the engine stores scripts.
        /// We use this as the root for our .cs files and .dll plugins.
        /// </summary>
        /// <returns>The filepath to the Scripts folder as a string, ex:
        /// <example>"C:\CryENGINE3\Game\Scripts\"</example></returns>
        public static string GetScriptsFolder()
        {
            return Path.Combine(GetGameFolder(), "Scripts");
        }
    }
}
