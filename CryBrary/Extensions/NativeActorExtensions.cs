using System;
using CryEngine;

namespace CryEngine.Native
{
    public static class NativeActorExtensions
    {
        public static IntPtr GetActorHandle(this ActorBase actor)
        {
            if (actor.IsDestroyed)
                throw new ScriptInstanceDestroyedException("Attempted to access native actor handle on a destroyed script");

            return actor.ActorHandle;
        }

        public static void SetActorHandle(this ActorBase actor, IntPtr handle)
        {
            actor.ActorHandle = handle;
        }
    }
}
