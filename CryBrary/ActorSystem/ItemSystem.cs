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

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _GiveItem(uint entityId, string itemClass);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _GiveEquipmentPack(uint entityId, string equipmentPack);

		public static void GiveItem(EntityId actorId, string itemClass)
		{
			_GiveItem(actorId._value, itemClass);
		}

		public static void GiveEquipmentPack(EntityId actorId, string equipmentPack)
		{
			_GiveEquipmentPack(actorId._value, equipmentPack);
		}
    }
}