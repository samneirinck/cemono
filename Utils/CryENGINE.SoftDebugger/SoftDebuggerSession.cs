using System;
using Mono.Debugging.Soft;
using System.Diagnostics;
using Mono.Debugging.Client;
namespace MonoDevelop.Debugger.Soft.CryENGINE
{
	public class CrySoftDebuggerSession : SoftDebuggerSession
	{
		protected override void OnRun(DebuggerStartInfo startInfo)
		{
			int debugPort;
			StartListening((CryDebuggerStartInfo)startInfo, out debugPort);
			
			// Start process
		}
		
		protected override void EndSession ()
		{
			// Stop process
			base.EndSession();
		}
	}
}

