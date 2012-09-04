using System;
using System.Runtime.CompilerServices;

namespace CryEngine.Native
{
	internal class NativeNetworkMethods : INativeNetworkMethods
	{
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void _RemoteInvocation(uint entityId, int scriptId, string methodName, object[] args, NetworkTarget target, int channelId);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static bool _IsServer();
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static bool _IsClient();
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static bool _IsMultiplayer();

		public void RemoteInvocation(uint entityId, int scriptId, string methodName, object[] args, NetworkTarget target, int channelId = -1)
		{
			_RemoteInvocation(entityId, scriptId, methodName, args, target, channelId);
		}

		public bool IsServer()
		{
			return _IsServer();
		}

		public bool IsClient()
		{
			return _IsClient();
		}

		public bool IsMultiplayer()
		{
			return _IsMultiplayer();
		}
	}
}