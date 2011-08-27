using System;
using System.Reflection;
namespace Cemono.Extensions
{
    public static class TypeExtensions
    {
        public static bool Implements(this Type thisType, Type baseType)
        {
            return baseType.IsAssignableFrom(thisType) && !thisType.Equals(baseType);
        }

        public static T GetAttribute<T>(this Type thisType) where T : Attribute
        {
            var attributes = thisType.GetCustomAttributes(typeof(T), true);

            if (attributes.Length > 0)
            {
                return (T)attributes[0];
            }
            else
            {
                return default(T);
            }
        }

        public static T GetAttribute<T>(this PropertyInfo propertyInfo) where T : Attribute
        {
            var attributes = propertyInfo.GetCustomAttributes(typeof(T), true);
            if (attributes.Length > 0)
            {
                return (T)attributes[0];
            }
            else
            {
                return default(T);
            }
        }
    }
}
