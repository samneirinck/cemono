using System;
using System.Runtime.CompilerServices;

namespace CryEngine.Native
{
    internal static class NativeSoundMethods
    {
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void SetGlobalParameter(string sGlobalParameter, float fValue);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static IntPtr CreateSound(string sGroupAndSoundName, uint nFlags, uint nFlagsExtended = 0);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static IntPtr CreateLineSound(string sGroupAndSoundName, uint nFlags, uint nFlagsExtended, Vec3 vStart, Vec3 vEnd);
		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static IntPtr CreateSphereSound(string sGroupAndSoundName, uint nFlags, uint nFlagsExtended, float fRadius);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void Play(IntPtr pSound, float fVolumeScale = 1.0f, bool bForceActiveState = true, bool bSetRatio = true, IntPtr ?pEntitySoundProxy = null);
		//[MethodImpl(MethodImplOptions.InternalCall)]
		//extern internal static void Stop(IntPtr pSound, ESoundStopMode eStopMode = ESoundStopMode_EventFade);

		[MethodImpl(MethodImplOptions.InternalCall)]
		extern internal static void SetPosition(IntPtr pSound, Vec3 position);
    }
}