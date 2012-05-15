namespace CryEngine
{
	public static class Convert
	{
		public static object FromString(EntityPropertyType type, string value)
		{
			if(value == null)
				throw new System.ArgumentNullException("value");

			switch(type)
			{
				case EntityPropertyType.Bool:
					return ChangeType(value, typeof(bool));
				case EntityPropertyType.Int:
					return ChangeType(value, typeof(int));
				case EntityPropertyType.Float:
					return ChangeType(value, typeof(float));
				case EntityPropertyType.String:
					return value;
				case EntityPropertyType.Vec3:
					return ChangeType(value, typeof(Vec3));
			}

			return null;
		}

		public static object ChangeType(object value, System.Type conversionType)
		{
			var valueType = value.GetType();

			if(conversionType.IsEnum && valueType == typeof(string))
				return System.Enum.Parse(conversionType, (string)value);
			else if(conversionType == typeof(Vec3) && valueType == typeof(string))
			{
				string[] split = ((string)value).Split(',');

				return new Vec3(System.Convert.ToSingle(split[0]), System.Convert.ToSingle(split[1]), System.Convert.ToSingle(split[2]));
			}

			return System.Convert.ChangeType(value, conversionType);
		}
	}
}
