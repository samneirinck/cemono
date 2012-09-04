using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CryEngine
{
	public class RemoteInvocationException : Exception
	{
		public RemoteInvocationException()
		{
		}

		public RemoteInvocationException(string message)
			: base(message)
		{
		}

		public RemoteInvocationException(string message, Exception inner)
			: base(message, inner)
		{
		}
	}
}
