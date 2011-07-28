using System;
using Mono.Debugging.Soft;
using System.Diagnostics;
namespace MonoDevelop.Debugger.Soft.Cemono
{
	public class CemonoSoftDebuggerSession : SoftDebuggerSession
	{
		protected override void OnRun (Mono.Debugging.Client.DebuggerStartInfo startInfo)
		{
			CemonoDebuggerStartInfo dsi = (CemonoDebuggerStartInfo) startInfo;
			
			StartGame(dsi);
			StartListening(dsi);
			
		}
		
		private void StartGame (CemonoDebuggerStartInfo dsi)
		{
			// TODO
		}
		
		private void StartListening(CemonoDebuggerStartInfo dsi)
		{
			// TODO		
		}
}
}

