using System.Runtime.CompilerServices;

namespace CryEngine
{
	/// <summary>
	/// ItemSystem contains scriptbinds used in relation to the item system.
	/// </summary>
    public class ItemSystem
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void CacheItemGeometry(string itemClass);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern public static void CacheItemSound(string itemClass);
    }
}