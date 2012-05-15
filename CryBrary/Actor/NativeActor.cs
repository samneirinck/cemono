
namespace CryEngine
{
	/// <summary>
	/// Used for non-CryMono actors.
	/// </summary>
	class NativeActor : Actor
	{
		public NativeActor() { }
		public NativeActor(EntityId id) { Id = id; }
	}
}
