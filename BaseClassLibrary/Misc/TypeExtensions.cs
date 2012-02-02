using System;
using System.Reflection;

/// <summary>
/// Contains useful extension methods used within the BCL.
/// </summary>
namespace CryEngine.Extensions
{
    public static class TypeExtensions
    {
        public static bool ContainsAttribute<T>(this Type type) where T : Attribute
        {
			return type.GetCustomAttributes(typeof(T), true).Length > 0;
        }

		public static bool ContainsAttribute<T>(this PropertyInfo info) where T : Attribute
		{
			return info.GetCustomAttributes(typeof(T), true).Length > 0;
		}

		public static bool ContainsAttribute<T>(this FieldInfo info) where T : Attribute
		{
			return info.GetCustomAttributes(typeof(T), true).Length > 0;
		}

		public static bool ContainsAttribute<T>(this MethodInfo info) where T : Attribute
		{
			return info.GetCustomAttributes(typeof(T), true).Length > 0;
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
				return null;
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
				return null;
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
				return null;
			}
		}

		public static bool TryGetAttribute<T>(this MethodInfo method, out T attribute) where T : Attribute
		{
			var attributes = method.GetCustomAttributes(typeof(T), true);

			if(attributes.Length > 0)
			{
				attribute = attributes[0] as T;
				return true;
			}
			else
			{
				attribute = null;
				return false;
			}
		}

        public static bool TryGetAttribute<T>(this MemberInfo member, out T attribute) where T : Attribute
        {
            var attributes = member.GetCustomAttributes(typeof(T), true);

            if (attributes.Length > 0)
            {
                attribute = attributes[0] as T;
                return true;
            }
            else
            {
                attribute = null;
                return false;
            }
        }
    }
}
