using System;
using System.Collections.Generic;

namespace CryEngine
{
	public delegate void ConsoleCommandDelegate(string[] args, string fullCommandLine);

	public static class ConsoleCommand
	{
		public static void Register(string name, ConsoleCommandDelegate func, string comment = "", CVarFlags flags = CVarFlags.None)
		{
			CVar._RegisterCommand(name, comment, flags);

			commands.Add(name, func);
		}

		internal static void OnCommand(string fullCommandLine)
		{
			var argsWithName = fullCommandLine.Split(' ');
			var name = argsWithName[0];

			var args = new string[argsWithName.Length - 1];
			for(int i = 1; i < argsWithName.Length; i++)
				args[i - 1] = argsWithName[i];

			commands[name](args, fullCommandLine);
		}

		static Dictionary<string, ConsoleCommandDelegate> commands = new Dictionary<string, ConsoleCommandDelegate>();
	}

	[AttributeUsage(AttributeTargets.Method)]
	public sealed class ConsoleCommandAttribute : Attribute
	{
		public string Name;
		public string Comment;
		public CVarFlags Flags;
	}
}
