using System;

namespace CryEngine
{
	[AttributeUsage(AttributeTargets.Method)]
	public sealed class RemoteInvocationAttribute : Attribute
	{
		public RemoteInvocationAttribute()
		{
			Target = NetworkTarget.Any;
		}

		public RemoteInvocationAttribute(NetworkTarget target)
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
