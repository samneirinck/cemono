using System.Runtime.CompilerServices;

namespace CryEngine
{
	/// <summary>
	/// Provides an direct interface to the 3DEngine.
	/// </summary>
	internal static class Engine
	{
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static float _GetTerrainElevation(float positionX, float positionY, bool includeOutdoorVoxels);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static int _GetTerrainSize();

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static int _GetTerrainSectorSize();

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static int _GetTerrainUnitSize();

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static int _GetTerrainZ(int x, int y);
	}
}
