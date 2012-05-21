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

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _SetTimeOfDay(float hour, bool forceUpdate = false);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static float _GetTimeOfDay();

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static TimeOfDay.AdvancedInfo _GetTimeOfDayAdvancedInfo();
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _SetTimeOfDayAdvancedInfo(TimeOfDay.AdvancedInfo advancedInfo);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _SetTimeOfDayVariableValue(TimeOfDay.ParamId id, float value);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _SetTimeOfDayVariableValueColor(TimeOfDay.ParamId id, Vec3 value);
	}
}
