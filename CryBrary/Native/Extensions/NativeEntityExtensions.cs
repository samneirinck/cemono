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
				throw new EntityDestroyedException("Attempted to access native entity handle on a destroyed entity");

			return entity.EntityHandleRef;
		}

		public static void SetEntityHandle(this EntityBase entity, HandleRef handleRef)
		{
			entity.EntityHandleRef = handleRef;
		}

		public static HandleRef GetAnimatedCharacterHandle(this EntityBase entity)
		{
			Debug.LogAlways("GetAnimatedCharacterHandle pre {0} on entity {1}", entity.AnimatedCharacterHandleRef.Handle, entity.Id);

			if (entity.IsDestroyed)
				throw new EntityDestroyedException("Attempted to access native animated character handle on a destroyed entity");
			if (entity.AnimatedCharacterHandleRef.Handle == IntPtr.Zero)
			{
				Debug.LogStackTrace();
				entity.SetAnimatedCharacterHandle(new HandleRef(entity, NativeMethods.Entity.AcquireAnimatedCharacter(entity.Id)));
			}

			Debug.LogAlways("GetAnimatedCharacterHandle post {0}", entity.AnimatedCharacterHandleRef.Handle);

			return entity.AnimatedCharacterHandleRef;
		}

		public static void SetAnimatedCharacterHandle(this EntityBase entity, HandleRef handleRef)
		{
			entity.AnimatedCharacterHandleRef = handleRef;
		}
	}
}
