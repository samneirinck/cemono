using System.Runtime.CompilerServices;

namespace CryEngine
{
	/// <summary>
	/// Provides an direct interface to the 3DEngine.
	/// Mildly WIP.
	/// </summary>
    public static class Engine
    {
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern public static float GetTerrainElevation(int x, int y, bool includeOutdoorVoxels = false);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern public static int GetTerrainSize();
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern public static int GetTerrainSectorSize();
    }
}
