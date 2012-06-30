using System.Runtime.CompilerServices;
using CryEngine.Native;

namespace CryEngine
{
	/// <summary>
	/// Provides an direct interface to the 3DEngine.
	/// </summary>
	public static class Engine
	{
        private static INative3DEngineMethods _native3DEngineMethods;
        internal static INative3DEngineMethods Native3DEngineMethods
        {
            get { return _native3DEngineMethods ?? (_native3DEngineMethods = new Native3DEngineMethods()); }
            set { _native3DEngineMethods = value; }
        }

        public static float GetTimeOfDay()
        {
            return Native3DEngineMethods.GetTimeOfDay();
        }

        public static int GetTerrainSize()
        {
            return Native3DEngineMethods.GetTerrainSize();
        }

        public static int GetTerrainUnitSize()
        {
            return Native3DEngineMethods.GetTerrainUnitSize();
        }

        public static float GetTerrainZ(int x, int y)
        {
            return Native3DEngineMethods.GetTerrainZ(x, y);
        }

        public static TimeOfDay.AdvancedInfo GetTimeOfDayAdvancedInfo()
        {
            return Native3DEngineMethods.GetTimeOfDayAdvancedInfo();
        }

        public static void SetTimeOfDayAdvancedInfo(TimeOfDay.AdvancedInfo info)
        {
            Native3DEngineMethods.SetTimeOfDayAdvancedInfo(info);
        }

        public static float GetTerrainElevation(float x, float y, bool checkVoxels)
        {
            return Native3DEngineMethods.GetTerrainElevation(x, y, checkVoxels);
        }

        public static void SetTimeOfDay(float value, bool forceUpdates)
        {
            Native3DEngineMethods.SetTimeOfDay(value, forceUpdates);
        }

        public static void SetTimeOfDayVariableValue(int id, float value)
        {
            Native3DEngineMethods.SetTimeOfDayVariableValue(id,value);
        }

        public static void SetTimeOfDayVariableValueColor(int id, Vec3 value)
        {
            Native3DEngineMethods.SetTimeOfDayVariableValueColor(id, value);
        }
    }
}
