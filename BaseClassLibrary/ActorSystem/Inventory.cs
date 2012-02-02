using System;
using System.Runtime.CompilerServices;

namespace CryEngine
{
    public class Inventory
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern protected static void _GiveItem(UInt32 entityId, string itemClass);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern protected static void _GiveEquipmentPack(UInt32 entityId, string equipmentPack);

        public Inventory(UInt32 ownerId)
        {
            OwnerId = ownerId;
        }

        public void GiveItem(string itemClass)
        {
            _GiveItem(OwnerId, itemClass);
        }

        public void GiveEquipmentPack(string equipmentPack)
        {
            _GiveEquipmentPack(OwnerId, equipmentPack);
        }

        UInt32 OwnerId;
    }
}
