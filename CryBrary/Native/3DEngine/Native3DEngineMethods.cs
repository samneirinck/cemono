using System;
using System.Runtime.CompilerServices;

namespace CryEngine.Native
{
    internal class Native3DEngineMethods : INative3DEngineMethods
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
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
        extern internal static void _SetTimeOfDayVariableValue(int id, float value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _SetTimeOfDayVariableValueColor(int id, Vec3 value);

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _ActivatePortal(Vec3 pos, bool activate, string entityName);

        public float GetTerrainElevation(float positionX, float positionY, bool includeOutdoorVoxels)
        {
            return _GetTerrainElevation(positionX, positionY, includeOutdoorVoxels);
        }

        public int GetTerrainSize()
        {
            return _GetTerrainSize();
        }

        public int GetTerrainSectorSize()
        {
            return _GetTerrainSectorSize();
        }

        public int GetTerrainUnitSize()
        {
            return _GetTerrainUnitSize();
        }

        public int GetTerrainZ(int x, int y)
        {
            return _GetTerrainZ(x, y);
        }

        public void SetTimeOfDay(float hour, bool forceUpdate = false)
        {
            _SetTimeOfDay(hour,forceUpdate);
        }
        
        public float GetTimeOfDay()
        {
            return _GetTimeOfDay();
        }

        public TimeOfDay.AdvancedInfo GetTimeOfDayAdvancedInfo()
        {
            return _GetTimeOfDayAdvancedInfo();
        }
        
        public void SetTimeOfDayAdvancedInfo(TimeOfDay.AdvancedInfo advancedInfo)
        {
            _SetTimeOfDayAdvancedInfo(advancedInfo);
        }
        
        public void SetTimeOfDayVariableValue(int id, float value)
        {
            _SetTimeOfDayVariableValue(id,value);
        }
        
        public void SetTimeOfDayVariableValueColor(int id, Vec3 value)
        {
            _SetTimeOfDayVariableValueColor(id, value);
        }

		public void ActivatePortal(Vec3 pos, bool activate, string entityName)
		{
			_ActivatePortal(pos, activate, entityName);
		}
    }
}
