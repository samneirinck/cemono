using System.Runtime.CompilerServices;
using System;
using System.Globalization;
using System.Runtime.InteropServices;
namespace CryEngine.API
{
    public class Console
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        [DllImport(Constants.DebuggingDll)]
        extern private static bool _GetCVar(string cvarName);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        [DllImport(Constants.DebuggingDll)]
        extern private static string _GetCVarHelpText(string cvarName);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        [DllImport(Constants.DebuggingDll)]
        extern private static int _GetCVarValueInt(string cvarName);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        [DllImport(Constants.DebuggingDll)]
        extern private static float _GetCVarValueFloat(string cvarName);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        [DllImport(Constants.DebuggingDll)]
        extern private static string _GetCVarValueString(string cvarName);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        [DllImport(Constants.DebuggingDll)]
        extern private static void _SetCVarValueInt(string cvarName, int value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        [DllImport(Constants.DebuggingDll)]
        extern private static void _SetCVarValueFloat(string cvarName, float value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        [DllImport(Constants.DebuggingDll)]
        extern private static void _SetCVarValueString(string cvarName, string value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        [DllImport(Constants.DebuggingDll)]
        extern private static int _GetCVarFlags(string cvarName);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        [DllImport(Constants.DebuggingDll)]
        extern private static void _SetCVarFlags(string cvarName, int flags);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        [DllImport(Constants.DebuggingDll)]
        extern private static void _RegisterCVarInt(string cvarName, int defaultValue, int flags, string help);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        [DllImport(Constants.DebuggingDll)]
        extern private static void _RegisterCVarFloat(string cvarName, float defaultValue, int flags, string help);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        [DllImport(Constants.DebuggingDll)]
        extern private static void _RegisterCVarString(string cvarName, string defaultValue, int flags, string help);


        /// <summary>
        /// Gets a cvar
        /// </summary>
        /// <typeparam name="T">int/float/string</typeparam>
        /// <param name="cvarName">Name of the cvar</param>
        /// <returns>CVar instance</returns>
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
        /// <param name="cvarName">Name of the cvar</param>
        /// <returns>Help text</returns>
        public static string GetCVarHelpText(string cvarName)
        {
            return _GetCVarHelpText(cvarName);
        }

        /// <summary>
        /// Gets the value of the cvar
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="cvarName">Name of the cvar</param>
        /// <returns>Cvar value</returns>
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
        /// <param name="cvarName">Name of the cvar</param>
        /// <param name="value">Cvar value</param>
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
        /// <param name="cvarName">Name of the cvar</param>
        /// <returns>Flags</returns>
        public static CVarFlags GetCVarFlags(string cvarName)
        {
            return (CVarFlags)_GetCVarFlags(cvarName);
        }

        /// <summary>
        /// Set the flags of a cvar
        /// </summary>
        /// <param name="cvarName">Name of the cvar</param>
        /// <param name="flags">Flags</param>
        public static void SetCVarFlags(string cvarName, CVarFlags flags)
        {
            _SetCVarFlags(cvarName, (int)flags);
        }

        /// <summary>
        /// Registers a cvar
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="cvarName">Name of the cvar</param>
        /// <param name="defaultValue">Default value</param>
        /// <param name="flags">Cvar flags</param>
        /// <param name="help">Help text</param>
        public static void RegisterCVar<T>(string cvarName, T defaultValue, CVarFlags flags, string help) where T : IConvertible
        {
            var type = typeof(T);

            if (type == typeof(int))
                _RegisterCVarInt(cvarName, Convert.ToInt32(defaultValue), (int)flags, help);
            else if (type == typeof(float))
                _RegisterCVarFloat(cvarName, Convert.ToSingle(defaultValue), (int)flags, help);
            else if (type == typeof(string))
                _RegisterCVarString(cvarName, Convert.ToString(defaultValue), (int)flags, help);

        }


    }
}
