using System;
using MonoDevelop.Core.Execution;
using Mono.Debugging.Client;
using System.Net;
namespace MonoDevelop.Debugger.Soft.Cemono
{
	public class CemonoSoftDebuggerEngine : IDebuggerEngine
	{
		public bool CanDebugCommand(ExecutionCommand command)
		{
			return true;
		}
		
		public DebuggerStartInfo CreateDebuggerStartInfo(ExecutionCommand command)
		{
			var msi = new CemonoDebuggerStartInfo("Cemono");
			msi.SetUserAssemblies(null);
			return msi;
		}
		
		public DebuggerSession CreateSession()
		{
			return new CemonoSoftDebuggerSession();
		}
		
		public ProcessInfo[] GetAttachableProcesses()
		{
			return new ProcessInfo[0];
		}

	}
	
	public class CemonoDebuggerStartInfo : RemoteDebuggerStartInfo
	{
		public CemonoDebuggerStartInfo(string appName)
			: base(appName, IPAddress.Loopback, 58740)
		{	
		}
	}
}

