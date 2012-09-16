using System.Runtime.InteropServices;
using CryEngine;

namespace CryEngine.Native
{
	public static class NativeActorExtensions
	{
		public static HandleRef GetActorHandle(this Actor actor)
		{
			if (actor.IsDestroyed)
				throw new EntityDestroyedException();

			return actor.ActorHandleRef;
		}

		public static void SetActorHandle(this Actor actor, HandleRef handleRef)
		{
			actor.ActorHandleRef = handleRef;
		}
	}
}
