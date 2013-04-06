using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CryEngine.Serialization
{
	public interface ICrySerializable
	{
		void Serialize(CrySerializer serializer);
		void Deserialize(CrySerializer serializer);
	}
}
