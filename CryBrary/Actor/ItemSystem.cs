using System.Runtime.CompilerServices;
using CryEngine.Native;

namespace CryEngine
{
	/// <summary>
	/// ItemSystem contains scriptbinds used in relation to the item system.
	/// </summary>
	public static class ItemSystem
	{
		public static void GiveItem(EntityId actorId, string itemClass)
		{
			NativeMethods.ItemSystem.GiveItem(actorId, itemClass);
		}

		public static void GiveEquipmentPack(EntityId actorId, string equipmentPack)
		{
            NativeMethods.ItemSystem.GiveEquipmentPack(actorId, equipmentPack);
		}
	}
}