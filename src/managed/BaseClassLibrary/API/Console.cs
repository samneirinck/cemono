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
        extern private static int _GetCVarValueInt(string cvarName);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern private static float _GetCVarValueFloat(string cvarName);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern private static string _GetCVarValueString(string cvarName);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern private static void _SetCVarValueInt(string cvarName, int value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern private static void _SetCVarValueFloat(string cvarName, float value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern private static void _SetCVarValueString(string cvarName, string value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern private static int _GetCVarFlags(string cvarName);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern private static void _SetCVarFlags(string cvarName, int flags);


        /// <summary>
        /// Gets a cvar
        /// </summary>
        /// <typeparam name="T">int/float/string</typeparam>
        /// <param name="cvarName">Name of the cvar</param>
        /// <returns></returns>
        public static CVar<T> GetCVar<T>(string cvarName) where T : IConvertible
        {
            if (_GetCVar(cvarName))
            {
                return new CVar<T>(cvarName);
            }
            else
            {
                return null;
            }
        }

        /// <summary>
        /// Gets the help text of a cvar
        /// </summary>
        /// <param name="cvarName"></param>
        /// <returns></returns>
        public static string GetCVarHelpText(string cvarName)
        {
            return _GetCVarHelpText(cvarName);
        }

        /// <summary>
        /// Gets the value of the cvar
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="cvarName"></param>
        /// <returns></returns>
        public static T GetCVarValue<T>(string cvarName)
        {
            var type = typeof(T);

            if (type == typeof(int))
                return (T)Convert.ChangeType(_GetCVarValueInt(cvarName), type);
            else if (type == typeof(float))
                return (T)Convert.ChangeType(_GetCVarValueFloat(cvarName), type);
            else if (type == typeof(string))
                return (T)Convert.ChangeType(_GetCVarValueString(cvarName), type);


            return default(T);
        }

        /// <summary>
        /// Sets a cvar value
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="cvarName"></param>
        /// <param name="value"></param>
        public static void SetCVarValue<T>(string cvarName, T value) where T : IConvertible
        {
            var type = typeof(T);
            var culture = CultureInfo.InvariantCulture;

            if (type == typeof(int))
                _SetCVarValueInt(cvarName, value.ToInt32(culture));
            else if (type == typeof(float))
                _SetCVarValueFloat(cvarName, value.ToSingle(culture));
            else if (type == typeof(string))
                _SetCVarValueString(cvarName, value.ToString(culture));

        }

        /// <summary>
        /// Get the flags of a cvar
        /// </summary>
        /// <param name="cvarName"></param>
        /// <returns></returns>
        public static CVarFlags GetCVarFlags(string cvarName)
        {
            return (CVarFlags)_GetCVarFlags(cvarName);
        }

        /// <summary>
        /// Set the flags of a cvar
        /// </summary>
        /// <param name="cvarName"></param>
        /// <param name="flags"></param>
        public static void SetCVarFlags(string cvarName, CVarFlags flags)
        {
            _SetCVarFlags(cvarName, (int)flags);
        }


    }
}
