using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CryEngine
{
	/// <summary>
	/// Used for non-CryMono entities, i.e. Lua / C++ such.
	/// </summary>
	internal class NativeEntity : Entity
	{
		public NativeEntity(EntityId id)
			: base(id)
		{
		}
	}
}
