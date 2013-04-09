using System.Runtime.CompilerServices;
using CryEngine.Native;

namespace CryEngine
{
	public enum CmdArgType
	{
		/// <summary>
		/// Argument was not preceeded by anything
		/// </summary>
		Normal = 0,

		/// <summary>
		/// Argument was preceeded by a minus sign
		/// </summary>
		Pre,	

		/// <summary>
		/// Argument was preceeded by a plus sign
		/// </summary>
		Post,

		/// <summary>
		/// Argument is the executable filename
		/// </summary>
		Executable,
	}

	public static class CryConsole
	{
		public static string GetCommandLineArg(string name, CmdArgType type)
		{
			return NativeCVarMethods.GetCmdArg(name, (int)type);
		}
	}
}
