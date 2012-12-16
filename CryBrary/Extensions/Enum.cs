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

        //checks if the value contains the provided type
        public static bool ContainsFlag<T>(this System.Enum type, T value)
        {
            var intValue = (int)(object)value;

            return ((int)(object)type & intValue) == intValue;
        }
    }
}
