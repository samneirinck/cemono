using System.Runtime.CompilerServices;

namespace CryEngine
{
	/// <summary>
	/// Provides an direct interface to the 3DEngine.
	/// Mildly WIP.
	/// </summary>
    public class Engine
    {
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _RenderWorld(int renderFlags, Camera camera);
    }
}
