using System;
using Mono.Debugging.Soft;
using System.Net;

namespace MonoDevelop.Debugger.Soft.CryENGINE
{
	public class CryDebuggerStartInfo : SoftDebuggerStartInfo
	{
		public const int DEBUG_PORT = 65432;

		public CryDebuggerStartInfo(string appName)
			: base(new SoftDebuggerListenArgs(appName, IPAddress.Loopback, DEBUG_PORT))
		{
			
		}
	}
}

