using System.Runtime.CompilerServices;

namespace CryEngine.Native
{
    internal class NativeItemSystemMethods
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void CacheItemGeometry(string itemClass);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void CacheItemSound(string itemClass);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void GiveItem(uint entityId, string itemClass);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void GiveEquipmentPack(uint entityId, string equipmentPack);
    }
}
