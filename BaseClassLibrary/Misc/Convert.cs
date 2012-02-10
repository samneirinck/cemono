namespace CryEngine
{
	public class Convert
	{
		public static object FromString(string type, string value)
		{
			switch (type)
			{
				case "Boolean":
					return System.Convert.ToBoolean(value);
				case "Int32":
					return System.Convert.ToInt32(value);
				case "Single":
					return System.Convert.ToSingle(value);
				case "UInt32":
					return System.Convert.ToUInt32(value);
				case "String":
					return value;
				case "Vec3":
					{
						string[] split = value.Split(',');
						return new Vec3(System.Convert.ToSingle(split[0]), System.Convert.ToSingle(split[1]), System.Convert.ToSingle(split[2]));
					}
			}

			return null;
		}

		public static object FromString(System.Type type, string value)
		{
			return FromString(type.Name, value);
		}

		public static object FromString(EntityPropertyType type, string value)
		{
			switch (type)
			{
				case EntityPropertyType.Bool:
					return FromString(typeof(bool), value);
				case EntityPropertyType.Int:
					return FromString(typeof(int), value);
				case EntityPropertyType.Float:
					return FromString(typeof(float), value);
				case EntityPropertyType.String:
					return value;
				case EntityPropertyType.Vec3:
						return FromString(typeof(Vec3), value);
			}

			return null;
		}
	}
}
