using System.Collections.Generic;

namespace CryEngine
{
	public static class CCommand
	{
		public delegate void CCommandDelegate(string[] args, string fullCommandLine);
		public static void Register(string name, CCommandDelegate func, string comment = "", CVarFlags flags = CVarFlags.None)
		{
			Console._RegisterCommand(name, comment, flags);

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

		static Dictionary<string, CCommandDelegate> commands = new Dictionary<string, CCommandDelegate>();
	}
}
