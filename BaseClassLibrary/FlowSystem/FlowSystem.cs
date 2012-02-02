using System;
using System.Runtime.CompilerServices;

namespace CryEngine
{
    public class FlowSystem
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void RegisterNode(string name, string category, bool isEntity);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _SetRegularlyUpdated(int scriptId, bool updated);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static bool _IsPortActive(int scriptId, int port);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _ActivateOutput(int scriptId, int port);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _ActivateOutputInt(int scriptId, int port, int value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _ActivateOutputFloat(int scriptId, int port, float value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _ActivateOutputEntityId(int scriptId, int port, uint value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _ActivateOutputString(int scriptId, int port, string value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _ActivateOutputBool(int scriptId, int port, bool value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _ActivateOutputVec3(int scriptId, int port, Vec3 value);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static int _GetPortValueInt(int scriptId, int port);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static float _GetPortValueFloat(int scriptId, int port);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static uint _GetPortValueEntityId(int scriptId, int port);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static string _GetPortValueString(int scriptId, int port);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static bool _GetPortValueBool(int scriptId, int port);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static Vec3 _GetPortValueVec3(int scriptId, int port);
    }
}