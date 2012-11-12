using System;
using CryEngine;

namespace CryEngine.Native
{
    public static class NativeEntityExtensions
    {
        public static IntPtr GetEntityHandle(this EntityBase entity)
        {
            if (entity.IsDestroyed)
                throw new ScriptInstanceDestroyedException("Attempted to access native entity handle on a destroyed script");

            return entity.EntityHandle;
        }

        public static void SetEntityHandle(this EntityBase entity, IntPtr handle)
        {
            entity.EntityHandle = handle;
        }

        public static IntPtr GetAnimatedCharacterHandle(this EntityBase entity)
        {
            if (entity.IsDestroyed)
                throw new ScriptInstanceDestroyedException("Attempted to access native animated character handle on a destroyed entity");
            if (entity.AnimatedCharacterHandle == IntPtr.Zero)
                entity.SetAnimatedCharacterHandle(NativeEntityMethods.AcquireAnimatedCharacter(entity.Id));

            return entity.AnimatedCharacterHandle;
        }

        public static void SetAnimatedCharacterHandle(this EntityBase entity, IntPtr handle)
        {
            entity.AnimatedCharacterHandle = handle;
        }
    }
}
