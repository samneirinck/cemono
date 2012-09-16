using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CryEngine
{
	public class EntityDestroyedException : Exception
	{
		public EntityDestroyedException()
		{
		}

		public EntityDestroyedException(string message)
			: base(message)
		{
		}

		public EntityDestroyedException(string message, Exception inner)
			: base(message, inner)
		{
		}
	}
}
