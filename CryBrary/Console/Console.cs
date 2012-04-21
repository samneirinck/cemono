using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace CryEngine
{
	public partial class Console
    {
        // Console commands
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _RegisterCommand(string name, string description, CVarFlags flags);

        // CVars
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _RegisterCVarFloat(string name, ref float val, float defaultVal, CVarFlags flags, string description);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _RegisterCVarInt(string name, ref int val, int defaultVal, CVarFlags flags, string description);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _RegisterCVarString(string name, [MarshalAs(UnmanagedType.LPStr)] string val, string defaultVal, CVarFlags flags, string description);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static float _GetCVarFloat(string name);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static int _GetCVarInt(string name);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static string _GetCVarString(string name);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _SetCVarFloat(string name, float value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _SetCVarInt(string name, int value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _SetCVarString(string name, string value);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static bool _HasCVar(string name);
    }
}
