using System;
using MonoDevelop.Core.Execution;
using Mono.Debugging.Client;
using System.Net;
using MonoDevelop.Debugger.Soft;

namespace MonoDevelop.Debugger.Soft.Cemono
{
	public class CemonoSoftDebuggerEngine : IDebuggerEngine
	{
		public bool CanDebugCommand(ExecutionCommand command)
		{
			return true;	
		}
		
		public DebuggerStartInfo CreateDebuggerStartInfo (ExecutionCommand cmd)
		{
			var msi = new CemonoDebuggerStartInfo("cemono");
			return msi;
		}
		
		public DebuggerSession CreateSession ()
		{
			return new CemonoSoftDebuggerSession();
		}
		
		public ProcessInfo[] GetAttachableProcesses ()
		{
			return new ProcessInfo[0];
		}
	}
}

