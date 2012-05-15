using System;
using System.Reflection;

namespace CryEngine.Extensions
{
	/// <summary>
	/// Useful extensions when working with reflection.
	/// </summary>
	public static class ReflectionExtensions
	{
		/// <summary>
		/// Determines whether this member is decorated with at least one instance of a given attribute.
		/// </summary>
		/// <typeparam name="T">The attribute to search for.</typeparam>
		/// <param name="info">The member on which the search is performed.</param>
		/// <returns>True if the member is decorated with at least one instance of attribute T.</returns>
		public static bool ContainsAttribute<T>(this MemberInfo info) where T : Attribute
		{
			return info.GetCustomAttributes(typeof(T), true).Length > 0;
		}

		/// <summary>
		/// Determines whether this method is decorated with at least one instance of a given attribute.
		/// </summary>
		/// <typeparam name="T">The attribute to search for.</typeparam>
		/// <param name="info">The method on which the search is performed.</param>
		/// <returns>True if the method is decorated with at least one instance of attribute T.</returns>
		public static bool ContainsAttribute<T>(this MethodInfo info) where T : Attribute
		{
			return info.GetCustomAttributes(typeof(T), true).Length > 0;
		}

		/// <summary>
		/// Determines whether a given type is the child of another.
		/// </summary>
		/// <param name="thisType">The child type.</param>
		/// <param name="baseType">The possible parent type.</param>
		/// <returns>True if thisType is a child of baseType.</returns>
		public static bool Implements(this Type thisType, Type baseType)
		{
			return baseType.IsAssignableFrom(thisType) && !thisType.Equals(baseType);
		}

		/// <summary>
		/// Gets the first instance of a given attribute on the selected member.
		/// </summary>
		/// <typeparam name="T">The attribute to search for.</typeparam>
		/// <param name="memberInfo">The member on which the search is performed.</param>
		/// <returns>The first instance of attribute T, or null if none is found.</returns>
		public static T GetAttribute<T>(this MemberInfo memberInfo) where T : Attribute
		{
			var attributes = memberInfo.GetCustomAttributes(typeof(T), true);
			if(attributes.Length > 0)
			{
				return (T)attributes[0];
			}
			else
			{
				return null;
			}
		}

		/// <summary>
		/// Gets the first instance of a given attribute on the selected method.
		/// </summary>
		/// <typeparam name="T">The attribute to search for.</typeparam>
		/// <param name="methodInfo">The method on which the search is performed.</param>
		/// <returns>The first instance of attribute T, or null if none is found.</returns>
		public static T GetAttribute<T>(this MethodInfo methodInfo) where T : Attribute
		{
			var attributes = methodInfo.GetCustomAttributes(typeof(T), true);
			if(attributes.Length > 0)
			{
				return (T)attributes[0];
			}
			else
			{
				return null;
			}
		}

		/// <summary>
		/// Tests whether the method is decorated with a given attribute, and if so, assigns it via the out variable.
		/// </summary>
		/// <typeparam name="T">The attribute to search for.</typeparam>
		/// <param name="method">The method on which the search is performed.</param>
		/// <param name="attribute">The out parameter to which the attribute will be assigned.</param>
		/// <returns>True if the attribute exists.</returns>
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

		/// <summary>
		/// Tests whether the member is decorated with a given attribute, and if so, assigns it via the out variable.
		/// </summary>
		/// <typeparam name="T">The attribute to search for.</typeparam>
		/// <param name="member">The member on which the search is performed.</param>
		/// <param name="attribute">The out parameter to which the attribute will be assigned.</param>
		/// <returns>True if the attribute exists.</returns>
		public static bool TryGetAttribute<T>(this MemberInfo member, out T attribute) where T : Attribute
		{
			var attributes = member.GetCustomAttributes(typeof(T), true);

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
	}
}
