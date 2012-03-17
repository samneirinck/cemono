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
        extern internal static float _GetTerrainElevation(int positionX, int positionY, bool includeOutdoorVoxels);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static int _GetTerrainSize();
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static int _GetTerrainSectorSize();

        public static float GetTerrainElevation(int positionX, int positionY, bool includeOutdoorVoxels = false)
        {
            return _GetTerrainElevation(positionX, positionY, includeOutdoorVoxels);
        }

        public static int TerrainSize { get { return _GetTerrainSize(); } }
        public static int TerrainSectorSize { get { return _GetTerrainSectorSize(); } }
    }
}
