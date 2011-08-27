using System;
using Mono.Debugging.Soft;
using System.Net;

namespace MonoDevelop.Debugger.Soft.Cemono
{
	public class CemonoDebuggerStartInfo : SoftDebuggerStartInfo
	{
		public const int DEBUG_PORT = 65432;
		
		public CemonoDebuggerStartInfo (string appName)
			: base(new SoftDebuggerListenArgs(appName, IPAddress.Loopback, DEBUG_PORT))
		{
			
		}
	}
}

