using System;
using System.Runtime.CompilerServices;

namespace CryEngine
{
	/// <summary>
	/// ItemSystem contains scriptbinds used in relation to the item system.
	/// </summary>
    public class ItemSystem
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern protected static void _CacheItemGeometry(string itemClass);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern protected static void _CacheItemSound(string itemClass);

        public static void CacheItemGeometry(string itemClass)
        {
            _CacheItemGeometry(itemClass);
        }

        public static void CacheItemSound(string itemClass)
        {
            _CacheItemSound(itemClass);
        }
    }
}