using System;
using System.Runtime.InteropServices;
using CryEngine;

namespace CryEngine.Native
{
	public static class NativeEntityExtensions
	{
		public static HandleRef GetEntityHandle(this EntityBase entity)
		{
			if (entity.IsDestroyed)
				throw new ScriptInstanceDestroyedException("Attempted to access native entity handle on a destroyed entity");

			return entity.EntityHandleRef;
		}

		public static void SetEntityHandle(this EntityBase entity, HandleRef handleRef)
		{
			entity.EntityHandleRef = handleRef;
		}

		public static HandleRef GetAnimatedCharacterHandle(this EntityBase entity)
		{
			if (entity.IsDestroyed)
				throw new ScriptInstanceDestroyedException("Attempted to access native animated character handle on a destroyed entity");
			if (entity.AnimatedCharacterHandleRef.Handle == IntPtr.Zero)
				entity.SetAnimatedCharacterHandle(new HandleRef(entity, NativeMethods.Entity.AcquireAnimatedCharacter(entity.Id)));

			return entity.AnimatedCharacterHandleRef;
		}

		public static void SetAnimatedCharacterHandle(this EntityBase entity, HandleRef handleRef)
		{
			entity.AnimatedCharacterHandleRef = handleRef;
		}
	}
}
