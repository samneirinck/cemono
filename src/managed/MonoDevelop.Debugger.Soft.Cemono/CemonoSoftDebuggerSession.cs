using System;
using Mono.Debugging.Soft;
using System.Diagnostics;
using Mono.Debugging.Client;
namespace MonoDevelop.Debugger.Soft.Cemono
{
	public class CemonoSoftDebuggerSession : SoftDebuggerSession
	{
		protected override void OnRun (DebuggerStartInfo startInfo)
		{
			var dsi = (CemonoDebuggerStartInfo)startInfo;
			
			int debugPort;
			StartListening(dsi, out debugPort);
			
			// Start process
		}
		
		protected override void EndSession ()
		{
			// Stop process
			base.EndSession();
		}
	}
}

