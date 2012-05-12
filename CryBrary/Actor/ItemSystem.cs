using System.Runtime.CompilerServices;

namespace CryEngine
{
    internal interface INativeItemSystemMethods
    {
        void CacheItemGeometry(string itemClass);
        void CacheItemSound(string itemClass);
        void _GiveItem(uint entityId, string itemClass);
        void _GiveEquipmentPack(uint entityId, string equipmentPack);
    }

    /// <summary>
    /// ItemSystem contains scriptbinds used in relation to the item system.
    /// </summary>
    public class ItemSystem
    {
        #region Native Methods
        private static INativeItemSystemMethods _methods;
        internal static INativeItemSystemMethods Methods
        {
            get
            {
                return _methods ?? (_methods = new NativeItemSystemMethods());
            }
            set
            {
                _methods = value;
            }
        }

        class NativeItemSystemMethods : INativeItemSystemMethods
        {
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            public extern void CacheItemGeometry(string itemClass);

            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            public extern void CacheItemSound(string itemClass);

            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            public extern void _GiveItem(uint entityId, string itemClass);

            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            public extern void _GiveEquipmentPack(uint entityId, string equipmentPack);
        }
        #endregion

        public static void GiveItem(EntityId actorId, string itemClass)
        {
            Methods._GiveItem(actorId, itemClass);
        }

        public static void GiveEquipmentPack(EntityId actorId, string equipmentPack)
        {
            Methods._GiveEquipmentPack(actorId, equipmentPack);
        }
    }
}