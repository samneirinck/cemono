
namespace CryEngine
{
	public static class Terrain
	{
		private static TerrainElevation _elevation;

		/// <summary>
		/// A read-only map of the terrain's elevation.
		/// </summary>
		public static TerrainElevation Elevation
		{
			get
			{
				return _elevation ?? (_elevation = new TerrainElevation());
			}
		}

		/// <summary>
		/// The size of the terrain in metres.
		/// </summary>
		public static int Size { get { return Engine._GetTerrainSize(); } }

		/// <summary>
		/// The size of each terrain unit.
		/// </summary>
		public static int UnitsPerMetre { get { return Engine._GetTerrainUnitSize(); } }

		/// <summary>
		/// The size of the terrain in units.
		/// </summary>
		/// <remarks>
		/// The terrain system calculates the overall size by multiplying this value by the units per metre setting. A map set to 1024 units at 2 metres per unit will have a size of 2048 metres.
		/// </remarks>
		public static int UnitSize { get { return Size / UnitsPerMetre; } }
	}

	public class TerrainElevation
	{
		internal TerrainElevation() { }

		/// <summary>
		/// Accesses the elevation at a specified location.
		/// </summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		/// <returns></returns>
		public float this[int x, int y]
		{
			get
			{
				return Engine._GetTerrainZ(x, y);
			}
		}

		/// <summary>
		/// Accesses the elevation at a specified location, with support for interpolated values.
		/// </summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		/// <returns></returns>
		public float this[float x, float y, bool checkVoxels = false]
		{
			get
			{
				return Engine._GetTerrainElevation(x, y, checkVoxels);
			}
		}
	}
}