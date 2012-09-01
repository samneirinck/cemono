using System.Runtime.InteropServices;

namespace CryEngine
{
	/// <summary>
	/// Used for non-CryMono actors.
	/// </summary>
	[ExcludeFromCompilation]
	class NativeActor : Actor
	{
		public NativeActor() { }

		public NativeActor(ActorInfo actorInfo)
		{
			Id = new EntityId(actorInfo.Id);
			HandleRef = new HandleRef(this, actorInfo.EntityPtr);
			ActorPointer = actorInfo.ActorPtr;
		}

		internal NativeActor(EntityId id)
		{
			Id = id;
		}
	}
}
