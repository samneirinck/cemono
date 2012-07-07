using System;
using System.Collections.Generic;

namespace CryEngine
{
	public delegate void ConsoleCommandDelegate(ConsoleCommandArgs e);

	public static class ConsoleCommand
	{
		public static void Register(string name, ConsoleCommandDelegate func, string comment = "", CVarFlags flags = CVarFlags.None)
		{
			if (!commands.ContainsKey(name))
			{
				CVar.RegisterCommand(name, comment, flags);

				commands.Add(name, func);
			}
		}

		internal static void OnCommand(string fullCommandLine)
		{
			var argsWithName = fullCommandLine.Split(' ');
			var name = argsWithName[0];

			var args = new string[argsWithName.Length - 1];
			for(int i = 1; i < argsWithName.Length; i++)
				args[i - 1] = argsWithName[i];

			commands[name](new ConsoleCommandArgs(name, args, fullCommandLine));
		}

		static Dictionary<string, ConsoleCommandDelegate> commands = new Dictionary<string, ConsoleCommandDelegate>();
	}

    public class ConsoleCommandArgs : EventArgs
    {
        public ConsoleCommandArgs(string name, string[] args, string fullCommandLine)
        {
            Name = name;
            Args = args;
            FullCommandLine = fullCommandLine;
        }

        public string Name { get; private set; }
        public string[] Args { get; private set; }
        public string FullCommandLine { get; private set; }
    }


	[AttributeUsage(AttributeTargets.Method)]
	public sealed class ConsoleCommandAttribute : Attribute
	{
		public string Name;
		public string Comment;
		public CVarFlags Flags;
	}
}
