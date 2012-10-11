using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CryEngine
{
	public class EntityRemovalException : Exception
	{
		public EntityRemovalException()
		{
		}

		public EntityRemovalException(string message)
			: base(message)
		{
		}

		public EntityRemovalException(string message, Exception inner)
			: base(message, inner)
		{
		}
	}
}
