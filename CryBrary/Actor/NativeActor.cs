using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CryEngine
{
	/// <summary>
	/// Used for non-CryMono actors.
	/// </summary>
	class NativeActor : Actor
	{
		public NativeActor() { }
		public NativeActor(EntityId id) { Id = id; }
	}
}
