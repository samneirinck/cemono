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
				throw new System.ArgumentException("Tried to set a negative entity ID");
		}

		#region Overrides
		public override bool Equals(object obj)
		{
			if(obj is EntityId)
				return (EntityId)obj == this;
			else if(obj is int)
				return (int)obj == _value;
			else if(obj is uint)
				return (uint)obj == _value;

			return false;
		}

		public override int GetHashCode()
		{
			return base.GetHashCode();
		}

		public override string ToString()
		{
			return string.Format("EntityId {0}", _value);
		}
		#endregion

		#region Operators
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
		#endregion
	}
}
