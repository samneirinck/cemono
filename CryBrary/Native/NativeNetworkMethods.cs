using System;
using System.Runtime.CompilerServices;

namespace CryEngine.Native
{
    internal class NativeNetworkMethods
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void RemoteInvocation(uint entityId, uint scriptId, string methodName, object[] args, NetworkTarget target, int channelId);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static bool IsServer();
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static bool IsClient();
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static bool IsMultiplayer();
    }
}