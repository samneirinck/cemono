using System;
using MonoDevelop.Core.Execution;
using Mono.Debugging.Client;
using System.Net;
using MonoDevelop.Debugger.Soft;

namespace MonoDevelop.Debugger.Soft.CryENGINE
{
	public class CrySoftDebuggerEngine : IDebuggerEngine
	{
		public bool CanDebugCommand(ExecutionCommand command)
		{
			return true;
		}
		
		public DebuggerStartInfo CreateDebuggerStartInfo (ExecutionCommand cmd)
		{
			return new CryDebuggerStartInfo("CryENGINE");
		}
		
		public DebuggerSession CreateSession ()
		{
			return new CrySoftDebuggerSession();
		}
		
		public ProcessInfo[] GetAttachableProcesses ()
		{
			return new ProcessInfo[0];
		}
	}
}

