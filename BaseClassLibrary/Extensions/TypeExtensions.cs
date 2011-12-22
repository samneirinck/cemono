using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Reflection;

/// <summary>Contains useful extension methods used within the BCL.</summary>
namespace CryEngine.Extensions
{
    public static class TypeExtensions
    {
        public static bool ContainsAttribute<T>(this Type type) where T : Attribute
        {
			var attributes = type.GetCustomAttributes(typeof(T), true);
            return attributes.Length > 0;
        }

		public static bool ContainsAttribute<T>(this PropertyInfo propertyInfo) where T : Attribute
		{
			var attributes = propertyInfo.GetCustomAttributes(typeof(T), true);
			return attributes.Length > 0;
		}

		public static bool ContainsAttribute<T>(this FieldInfo fieldInfo) where T : Attribute
		{
			var attributes = fieldInfo.GetCustomAttributes(typeof(T), true);
			return attributes.Length > 0;
		}

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

		public static T GetAttribute<T>(this FieldInfo fieldInfo) where T : Attribute
		{
			var attributes = fieldInfo.GetCustomAttributes(typeof(T), true);
			if(attributes.Length > 0)
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
