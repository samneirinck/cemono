using System.Runtime.CompilerServices;

namespace CryEngine
{
	public static class ParticleSystem
	{
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static int FindEffect(string effectName, bool loadResources = true);
	}
}