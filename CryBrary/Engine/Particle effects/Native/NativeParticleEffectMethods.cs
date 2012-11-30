using System;
using System.Runtime.CompilerServices;

namespace CryEngine.Native
{
    internal static class NativeParticleEffectMethods
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static IntPtr FindEffect(string effectName, bool loadResources);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void Spawn(IntPtr ptr, bool independent, Vec3 pos, Vec3 dir, float scale);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void Remove(IntPtr ptr);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void LoadResoruces(IntPtr ptr);
    }
}
