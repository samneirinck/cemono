using System;

namespace CryEngine
{
	[AttributeUsage(AttributeTargets.Method)]
	public sealed class NetworkCallAttribute : Attribute
	{
		public NetworkCallAttribute(NetworkTarget target)
		{
			Target = target;
		}

		public NetworkTarget Target;
	}

	public enum NetworkTarget
	{
		Any,
		Server,
		Client
	}
}
