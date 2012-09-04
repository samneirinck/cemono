using System;

namespace CryEngine
{
	[AttributeUsage(AttributeTargets.Method)]
	public sealed class RemoteInvocationAttribute : Attribute
	{
		public RemoteInvocationAttribute()
		{
		}
	}

	[Flags]
	public enum NetworkTarget
	{
		ToClientChannel = 0x01,
		ToOwnClient = 0x02,
		ToOtherClients = 0x04,
		ToAllClients = 0x08,

		ToServer = 0x100,

		NoLocalCalls = 0x10000,
		NoRemoteCalls = 0x20000,

		ToRemoteClients = NoLocalCalls | ToAllClients
	}
}
