using System;
using System.Collections.Generic;
using System.Linq;

using CryEngine.Initialization;

namespace CryEngine.Extensions
{
	public static class EnumExtensions
	{
		public static IEnumerable<T> GetMembers<T>()
		{
			return from value in Enum.GetNames(typeof(T))
				   select (T)Enum.Parse(typeof(T), value);
		}

		public static bool ContainsFlag(this ScriptType scriptType, ScriptType flag)
		{
			return (scriptType & flag) == flag;
		}
	}
}
