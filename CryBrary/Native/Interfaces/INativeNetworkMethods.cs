using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CryEngine.Native
{
	internal interface INativeNetworkMethods
	{
		void RemoteInvocation(uint entityId, int scriptId, string methodName, object[] args, NetworkTarget target, int channelId = 0);

		bool IsServer();
		bool IsClient();
		bool IsMultiplayer();
	}
}