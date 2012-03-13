using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using CryEngine.Extensions;

namespace CryEngine
{
	[AttributeUsage(AttributeTargets.Property)]
	public sealed class CVarAttribute : Attribute
	{
		[CVar("mono_string", "sup")] 
		public static string StringCVarTest { get; set; }

		[CVar("mono_int", 10)]
		public static int IntCVarTest { get; set; }

		[CVar("mono_float", 10f)]
		public static float FloatCVarTest { get; set; }

		public string Name { get; private set; }
		public int? IntValue { get; private set; }
		public float? FloatValue { get; private set; }
		public string StringValue { get; private set; }

		private CVarAttribute(string name)
		{
			Name = name;
		}

		public CVarAttribute(string name, int defaultValue)
			: this(name)
		{
			IntValue = defaultValue;
		}

		public CVarAttribute(string name, float defaultValue)
			: this(name)
		{
			FloatValue = defaultValue;
		}

		public CVarAttribute(string name, string defaultValue)
			: this(name)
		{
			StringValue = defaultValue;
		}

		public static void RegisterCVars(Assembly target)
		{
			foreach(var type in target.GetTypes())
			{
				CVarAttribute attr;
				foreach(var property in type.GetProperties().Where(prop => prop.TryGetAttribute(out attr)))
				{

				}
			}
		}
	}
}