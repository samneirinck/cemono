﻿using System;
using System.IO;

using CryEngine.Initialization;

namespace CryEngine
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
        public static string GetRootFolder()
        {
            // are we sure that this works all of the time? :p
            return Directory.GetCurrentDirectory();
        }

		/// <summary>
		/// Gets the folder in which script dumps are stored.
		/// </summary>
		/// <returns></returns>
		internal static string GetScriptDumpFolder()
		{
			return Path.Combine(GetRootFolder(), "Temp", "ScriptDumps");
		}

        /// <summary>
        /// Gets the current folder containing binaries relevant to current build type (Bin32 / Bin64).
        /// </summary>
        /// <returns>The filepath to the Binary folder as a string, ex:
        /// <example>"C:\CryENGINE3\Bin32"</example></returns>
        public static string GetBinaryFolder()
        {
            return Path.Combine(GetRootFolder(), "Bin" + (IntPtr.Size * 8).ToString());
        }

        /// <summary>
        /// Gets the engine folder.
        /// CryENGINE uses this store engine content which is not specific to the game being developed, i.e. shaders and configuration files.
        /// </summary>
        /// <returns>The filepath to the Engine folder as a string, ex:
        /// <example>"C:\CryENGINE3\Engine"</example></returns>
        public static string GetEngineFolder()
        {
            return Path.Combine(GetRootFolder(), "Engine");
        }

        /// <summary>
        /// Gets the current game folder.
        /// CryENGINE uses this as the root for all game-related content.
        /// </summary>
        /// <returns>The filepath to the Game folder as a string, ex:
        /// <example>"C:\CryENGINE3\Game"</example></returns>
        public static string GetGameFolder()
        {
            CVar cvar;
			if(CVar.TryGet("sys_game_folder", out cvar))
                return Path.Combine(GetRootFolder(), cvar.String);

            return Path.Combine(GetRootFolder(), "Game");
        }

        /// <summary>
        /// Gets the folder in which the engine stores scripts.
        /// We use this as the root for our .cs files and .dll plugins.
        /// </summary>
        /// <returns>The filepath to the Scripts folder as a string, ex:
        /// <example>"C:\CryENGINE3\Game\Scripts"</example></returns>
        public static string GetScriptsFolder()
        {
            return Path.Combine(GetGameFolder(), "Scripts");
        }

        /// <summary>
        /// Gets the folder in which a certain type of scripts are located.
        /// </summary>
        /// <param name="scriptType"></param>
        /// <returns>The folder in which scripts of this type are located; i.e. Game\Scripts\Entities</returns>
        /// <example>GetScriptFolder(MonoScriptType.Entity)</example>
        public static string GetScriptFolder(ScriptType scriptType)
        {
            string folder = "";
            switch (scriptType)
            {
                case ScriptType.Actor:
                    folder = @"Entities/Actor";
                    break;
                case ScriptType.Entity:
                    folder = "Entities";
                    break;
                case ScriptType.FlowNode:
                    folder = "FlowNodes";
                    break;
                case ScriptType.GameRules:
                    folder = "GameRules";
                    break;
				case ScriptType.EditorForm:
					folder = "EditorForms";
					break;
				case ScriptType.UIEvent:
					folder = "UI";
					break;
            }

            return Path.Combine(PathUtils.GetScriptsFolder(), folder);
        }

		public static string GetTempFolder()
		{
			return Path.Combine(Path.GetTempPath(), "CryMono");
		}
    }
}