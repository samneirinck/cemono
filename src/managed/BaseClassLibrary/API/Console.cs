using System.Runtime.CompilerServices;
using System;
using System.Globalization;
namespace Cemono.API
{
    public class Console
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern private static bool _GetCVar(string cvarName);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern private static string _GetCVarHelpText(string cvarName);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern private static object _GetCVarValue(string cvarName);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern private static void _SetCvarValueInt(string cvarName, int value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern private static void _SetCvarValueFloat(string cvarName, float value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern private static void _SetCvarValueString(string cvarName, string value);

        public static CVar<T> GetCVar<T>(string cvarName) where T : IConvertible
        {
            if (_GetCVar(cvarName))
            {
                return new CVar<T>(cvarName);
            }
            else
            {
                // TODO
                return null;
            }
        }

        public static string GetCVarHelpText(string cvarName)
        {
            return _GetCVarHelpText(cvarName);
        }

        public static T GetCVarValue<T>(string cvarName)
        {
            object returnValue = _GetCVarValue(cvarName);

            return (T)returnValue;
        }

        public static void SetCVarValue<T>(string cvarName, T value) where T : IConvertible
        {
            var type = typeof(T);
            var culture = CultureInfo.InvariantCulture;

            if (type == typeof(int))
                _SetCvarValueInt(cvarName, value.ToInt32(culture));
            else if (type == typeof(float))
                _SetCvarValueFloat(cvarName, value.ToSingle(culture));
            else if (type == typeof(string))
                _SetCvarValueString(cvarName, value.ToString(culture));

        }

        public static CVarFlags GetCVarFlags(string cvarName)
        {
            return CVarFlags.VF_BITFIELD;
        }

        public static void SetCVarFlags(string cvarName, CVarFlags flags)
        {
        }


    }
}
