using System;

namespace CryEngine.Native
{
    internal interface INative3DEngineMethods
    {
        float GetTerrainElevation(float positionX, float positionY, bool includeOutdoorVoxels);
        int GetTerrainSize();
        int GetTerrainSectorSize();
        int GetTerrainUnitSize();
        int GetTerrainZ(int x, int y);
        void SetTimeOfDay(float hour, bool forceUpdate = false);
        float GetTimeOfDay();
        TimeOfDay.AdvancedInfo GetTimeOfDayAdvancedInfo();
        void SetTimeOfDayAdvancedInfo(TimeOfDay.AdvancedInfo advancedInfo);
        void SetTimeOfDayVariableValue(int id, float value);
        void SetTimeOfDayVariableValueColor(int id, Vec3 value);

		IntPtr CreateLightSource();
		void SetLightSourceParams(IntPtr lightSourcePtr, LightParams lightParams);
		LightParams GetLightSourceParams(IntPtr lightSourcePtr);
    }
}