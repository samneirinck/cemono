using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CryEngine.Serialization
{
	[CLSCompliant(false)]
	public interface ICrySerializable
	{
		void Serialize(ICrySerialize serializer);
	}
}