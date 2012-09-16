using System.Runtime.InteropServices;

namespace CryEngine
{
	/// <summary>
	/// Represents an actor with a custom IActor implementation outside of CryMono.dll.
	/// </summary>
	[ExcludeFromCompilation]
	public class NativeActor : Actor
	{
		public NativeActor() { }

		internal NativeActor(ActorInfo actorInfo)
		{
			Id = new EntityId(actorInfo.Id);
			SetEntityHandle(actorInfo.EntityPtr);
			SetActorHandle(actorInfo.ActorPtr);
		}

		internal NativeActor(EntityId id)
		{
			Id = id;
		}

		public override void UpdateView(ref ViewParams viewParams)
		{
			throw new System.NotImplementedException();
		}
	}
}
