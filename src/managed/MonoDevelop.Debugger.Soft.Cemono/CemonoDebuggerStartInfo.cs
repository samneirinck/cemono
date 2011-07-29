using System;
using Mono.Debugging.Soft;
using System.Net;

namespace MonoDevelop.Debugger.Soft.Cemono
{
	public class CemonoDebuggerStartInfo : SoftDebuggerStartInfo
	{
		
		
		public CemonoDebuggerStartInfo (string appName)
			: base(new SoftDebuggerListenArgs(appName, IPAddress.Loopback, 65432))
		{
			
		}
	}
}

