using System;
using System.IO;
using System.Collections.Generic;

using CryEngine;

namespace CryMono
{
	/// <summary>
	/// This class monitors the file system and reloads the C# game code in reponse.
	/// </summary>
	public static class CryFileMonitor
	{
		static FileSystemWatcher csWatcher;
		static FileSystemWatcher pluginWatcher;

		static bool _enabled;
		/// <summary>
		/// Enables and disables the automatic script/plugin reloading.
		/// </summary>
		public static bool Enabled
		{
			get
			{
				return _enabled;
			}
			set
			{
				_enabled = value;

				if(value)
				{
					if(csWatcher == null)
					{
						csWatcher = new FileSystemWatcher(CryPath.GetScriptsFolder(), "*.cs");
						
						csWatcher.Changed += OnScriptChanged;
						csWatcher.Created += OnScriptChanged;
						csWatcher.Deleted += OnScriptChanged;
						csWatcher.Renamed += OnScriptChanged;

						csWatcher.Error += OnMonitorError;

						csWatcher.IncludeSubdirectories = true;
					}

					if(pluginWatcher == null)
					{
						pluginWatcher = new FileSystemWatcher(CryPath.GetScriptsFolder(), "*.dll");

						pluginWatcher.Changed += OnPluginChanged;
						pluginWatcher.Created += OnPluginChanged;
						pluginWatcher.Deleted += OnPluginChanged;
						pluginWatcher.Renamed += OnPluginChanged;

						pluginWatcher.Error += OnMonitorError;

						pluginWatcher.IncludeSubdirectories = true;
					}

					pluginWatcher.EnableRaisingEvents = true;
					csWatcher.EnableRaisingEvents = true;

					CryConsole.Log("Mono file system monitoring enabled.");
				}
				else
				{
					csWatcher.EnableRaisingEvents = false;
					pluginWatcher.EnableRaisingEvents = false;
					CryConsole.Log("Mono file system monitoring disabled.");
				}
			}
		}

		static void OnScriptChanged(object source, FileSystemEventArgs e)
		{
			CryConsole.Log("Script change in {0}, type {1}.", e.Name, e.ChangeType.ToString());
			Manager.scriptCompiler.Reload();
		}

		static void OnPluginChanged(object source, FileSystemEventArgs e)
		{
			CryConsole.Log("Plugin change in {0}, type {1}.", e.Name, e.ChangeType.ToString());
			Manager.scriptCompiler.Reload();
		}

		static void OnMonitorError(object source, ErrorEventArgs e)
		{
			CryConsole.LogException(e.GetException());
		}
	}
}
