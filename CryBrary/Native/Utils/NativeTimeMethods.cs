using System.Runtime.CompilerServices;

namespace CryEngine.Native
{
    internal class NativeTimeMethods : INativeTimeMethods
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern internal static void _SetTimeScale(float scale);

        public void SetTimeScale(float scale)
        {
            _SetTimeScale(scale);
        }
    }
}
