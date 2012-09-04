using System;
using System.Runtime.CompilerServices;

namespace CryEngine.Native
{
	internal class NativeNetworkMethods : INativeNetworkMethods
	{
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _RemoteInvocation(uint entityId, int scriptId, string methodName, object[] args, NetworkTarget target, int channelId);

		public void RemoteInvocation(uint entityId, int scriptId, string methodName, object[] args, NetworkTarget target, int channelId = -1)
		{
			_RemoteInvocation(entityId, scriptId, methodName, args, target, channelId);
		}
	}
}