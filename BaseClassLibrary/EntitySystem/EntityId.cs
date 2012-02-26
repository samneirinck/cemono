using System;

namespace CryEngine
{
	/// <summary>
	/// Entity ID's store references to game entities as unsigned integers. This class wraps that functionality for CLS compliance.
	/// </summary>
	public struct EntityId
	{
		internal uint _value;

		public EntityId(int id)
		{
			if(id >= 0)
				_value = (uint)id;
			else
				throw new ArgumentException("Tried to set a negative entity ID");
		}

		public static implicit operator int(EntityId id)
		{
			return (int)id._value;
		}

		public static implicit operator EntityId(int value)
		{
			return new EntityId(value);
		}

		[System.CLSCompliant(false)]
		public static implicit operator uint(EntityId id)
		{
			return id._value;
		}

		[System.CLSCompliant(false)]
		public static implicit operator EntityId(uint value)
		{
			return new EntityId { _value = value };
		}
	}
}
