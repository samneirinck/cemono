namespace CryEngine
{
	public struct BoundingBox
	{
		public BoundingBox(Vec3 min, Vec3 max)
			: this()
		{
			this.Min = min;
			this.Max = max;
		}

		#region Overrides
		public override string ToString()
		{
			return string.Format("min: {0} max: {1}", Min.ToString(), Max.ToString());
		}

		public override bool Equals(object obj)
		{
			if(obj is BoundingBox)
				return (BoundingBox)obj == this;

			return false;
		}

		public override int GetHashCode()
		{
			return base.GetHashCode();
		}
		#endregion

		#region Operators
		public static bool operator ==(BoundingBox b1, BoundingBox b2)
		{
			return b1.Min == b2.Min && b1.Max == b2.Max;
		}

		public static bool operator !=(BoundingBox b1, BoundingBox b2)
		{
			return b1.Min != b2.Min || b1.Max != b2.Max;
		}
		#endregion



		public Vec3 Min { get; set; }
		public Vec3 Max { get; set; }
	}
}
