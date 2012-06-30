using System.Runtime.CompilerServices;

namespace CryEngine.Native
{
    internal class NativeItemSystemMethods : INativeItemSystemMethods
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void _CacheItemGeometry(string itemClass);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _CacheItemSound(string itemClass);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _GiveItem(uint entityId, string itemClass);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _GiveEquipmentPack(uint entityId, string equipmentPack);

        public void GiveItem(uint entityId, string itemClass)
        {
            _GiveItem(entityId, itemClass);
        }

        public void GiveEquipmentPack(uint entityId, string equipmentPack)
        {
            _GiveEquipmentPack(entityId, equipmentPack);
        }

        public void CacheItemGeometry(string itemClass)
        {
            _CacheItemGeometry(itemClass);
        }
        public void CacheItemSound(string itemClass)
        {
            _CacheItemSound(itemClass);
        }
    }
}
