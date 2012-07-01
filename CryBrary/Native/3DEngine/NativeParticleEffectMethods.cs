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

        public IntPtr FindEffect(string effectName, bool loadResources)
        {
            return _FindEffect(effectName, loadResources);
        }

        public void Spawn(IntPtr ptr, bool independent, Vec3 pos, Vec3 dir, float scale)
        {
            _Spawn(ptr,independent,pos,dir,scale);
        }
    }
}
