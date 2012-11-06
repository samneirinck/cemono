using System.Runtime.InteropServices;

using CryEngine.Native;

namespace CryEngine
{
    /// <summary>
    /// Represents an actor with a custom IActor implementation outside of CryMono.dll.
    /// </summary>
    [ExcludeFromCompilation]
    public class NativeActor : ActorBase
    {
        public NativeActor() { }

        internal NativeActor(ActorInfo actorInfo)
        {
            Id = new EntityId(actorInfo.Id);
            this.SetEntityHandle(actorInfo.EntityPtr);
            this.SetActorHandle(actorInfo.ActorPtr);
        }

        internal NativeActor(EntityId id)
        {
            Id = id;
        }
    }
}
