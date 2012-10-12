using System.Runtime.CompilerServices;
using CryEngine.Native;

namespace CryEngine
{
	/// <summary>
	/// Provides an direct interface to the 3DEngine.
	/// </summary>
	public static class Engine
	{
        public static int GetTerrainSize()
        {
            return NativeMethods.Engine3D.GetTerrainSize();
        }

        public static int GetTerrainUnitSize()
        {
            return NativeMethods.Engine3D.GetTerrainUnitSize();
        }

        public static float GetTerrainZ(int x, int y)
        {
            return NativeMethods.Engine3D.GetTerrainZ(x, y);
        }

        public static TimeOfDay.AdvancedInfo GetTimeOfDayAdvancedInfo()
        {
            return NativeMethods.Engine3D.GetTimeOfDayAdvancedInfo();
        }

        public static void SetTimeOfDayAdvancedInfo(TimeOfDay.AdvancedInfo info)
        {
            NativeMethods.Engine3D.SetTimeOfDayAdvancedInfo(info);
        }

        public static float GetTerrainElevation(float x, float y, bool checkVoxels)
        {
            return NativeMethods.Engine3D.GetTerrainElevation(x, y, checkVoxels);
        }

        public static void SetTimeOfDay(float value, bool forceUpdates)
        {
            NativeMethods.Engine3D.SetTimeOfDay(value, forceUpdates);
        }

        public static void SetTimeOfDayVariableValue(int id, float value)
        {
            NativeMethods.Engine3D.SetTimeOfDayVariableValue(id, value);
        }

        public static void SetTimeOfDayVariableValueColor(int id, Vec3 value)
        {
            NativeMethods.Engine3D.SetTimeOfDayVariableValueColor(id, value);
        }

		public static void ActivatePortal(Vec3 pos, bool activate, string entityName)
		{
			NativeMethods.Engine3D.ActivatePortal(pos, activate, entityName);
		}
    }
}
