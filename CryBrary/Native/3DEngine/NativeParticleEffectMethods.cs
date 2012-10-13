using System;
using System.Runtime.CompilerServices;

namespace CryEngine.Native
{
    internal class NativeParticleEffectMethods : INativeParticleEffectMethods
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static IntPtr _FindEffect(string effectName, bool loadResources);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _Spawn(IntPtr ptr, bool independent, Vec3 pos, Vec3 dir, float scale);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _Remove(IntPtr ptr);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _LoadResoruces(IntPtr ptr);

        public IntPtr FindEffect(string effectName, bool loadResources)
        {
            return _FindEffect(effectName, loadResources);
        }

        public void Spawn(IntPtr ptr, bool independent, Vec3 pos, Vec3 dir, float scale)
        {
            _Spawn(ptr,independent,pos,dir,scale);
        }

		public void Remove(IntPtr ptr)
		{
			_Remove(ptr);
		}

		public void LoadResoruces(IntPtr ptr)
		{
			_LoadResoruces(ptr);
		}
    }
}
