using CryEngine.Native;

namespace CryEngine
{
	public static class Terrain
	{
		public static float GetTerrainElevation(int x, int y)
		{
			return NativeMethods.Engine3D.GetTerrainZ(x, y);
		}

		public static float GetTerrainElevation(float x, float y, bool checkVoxels = false)
		{
			return NativeMethods.Engine3D.GetTerrainElevation(x, y, checkVoxels);
		}

		/// <summary>
		/// The size of the terrain in metres.
		/// </summary>
		public static int Size { get { return NativeMethods.Engine3D.GetTerrainSize(); } }

		/// <summary>
		/// The size of each terrain unit.
		/// </summary>
		public static int UnitsPerMetre { get { return NativeMethods.Engine3D.GetTerrainUnitSize(); } }

		/// <summary>
		/// The size of the terrain in units.
		/// </summary>
		/// <remarks>
		/// The terrain system calculates the overall size by multiplying this value by the units per metre setting. A map set to 1024 units at 2 metres per unit will have a size of 2048 metres.
		/// </remarks>
		public static int UnitSize { get { return Size / UnitsPerMetre; } }
	}
}