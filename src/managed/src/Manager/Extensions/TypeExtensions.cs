using System;
namespace Cemono.Extensions
{
    public static class TypeExtensions
    {
        public static bool Implements(this Type thisType, Type baseType)
        {
            return baseType.IsAssignableFrom(thisType) && !thisType.Equals(baseType);
        }
    }
}
