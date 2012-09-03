namespace CryEngine
{
	public static class Convert
	{
		public static object FromString(EntityPropertyType type, string value)
		{
			if(type == EntityPropertyType.String)
				return value;
#if ((RELEASE && RELEASE_ENABLE_CHECKS) || !RELEASE)
			if(value == null)
				throw new System.ArgumentNullException("value");
			if(value.Length < 1)
				throw new System.ArgumentException("value string was empty");
#endif

			switch(type)
			{
				case EntityPropertyType.Bool:
					{
						if(value == "0")
							value = "false";
						else if(value == "1")
							value = "true";

						return bool.Parse(value);
					}
				case EntityPropertyType.Int:
					return int.Parse(value);
				case EntityPropertyType.Float:
					return float.Parse(value);
				case EntityPropertyType.Vec3:
					return Vec3.Parse(value);
			}

			return null;
		}
	}
}
