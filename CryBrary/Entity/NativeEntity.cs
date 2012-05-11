namespace CryEngine
{
	/// <summary>
	/// Used for non-CryMono entities, i.e. Lua / C++ such.
	/// </summary>
	internal class NativeEntity : Entity
	{
		public NativeEntity() { }
		public NativeEntity(EntityId id) { Id = id; }
	}
}
