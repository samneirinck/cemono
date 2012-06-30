using System.Runtime.CompilerServices;
using CryEngine.Native;

namespace CryEngine
{
	/// <summary>
	/// ItemSystem contains scriptbinds used in relation to the item system.
	/// </summary>
	public static class ItemSystem
	{
        private static INativeItemSystemMethods _itemSystemMethods;
        internal static INativeItemSystemMethods NativeActorSystemMethods
        {
            get { return _itemSystemMethods ?? (_itemSystemMethods = new NativeItemSystemMethods()); }
            set { _itemSystemMethods = value; }
        }

		public static void GiveItem(EntityId actorId, string itemClass)
		{
			NativeActorSystemMethods.GiveItem(actorId, itemClass);
		}

		public static void GiveEquipmentPack(EntityId actorId, string equipmentPack)
		{
            NativeActorSystemMethods.GiveEquipmentPack(actorId, equipmentPack);
		}
	}
}