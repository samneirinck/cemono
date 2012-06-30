namespace CryEngine.Native
{
    internal interface INativeItemSystemMethods
    {
        void GiveItem(uint entityId, string itemClass);
        void GiveEquipmentPack(uint entityId, string equipmentPack);
        void CacheItemGeometry(string itemClass);
        void CacheItemSound(string itemClass);
    }
}