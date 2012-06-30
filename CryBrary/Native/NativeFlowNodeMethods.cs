using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

namespace CryEngine.Native
{
    internal class NativeFlowNodeMethods : INativeFlowNodeMethods
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _RegisterNode(string typeName);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static IntPtr _GetNode(UInt32 graphId, UInt16 nodeId);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _SetRegularlyUpdated(IntPtr nodePtr, bool updated);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static bool _IsPortActive(IntPtr nodePtr, int port);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _ActivateOutput(IntPtr nodePtr, int port);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _ActivateOutputInt(IntPtr nodePtr, int port, int value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _ActivateOutputFloat(IntPtr nodePtr, int port, float value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _ActivateOutputEntityId(IntPtr nodePtr, int port, uint value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _ActivateOutputString(IntPtr nodePtr, int port, string value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _ActivateOutputBool(IntPtr nodePtr, int port, bool value);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static void _ActivateOutputVec3(IntPtr nodePtr, int port, Vec3 value);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static int _GetPortValueInt(IntPtr nodePtr, int port);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static float _GetPortValueFloat(IntPtr nodePtr, int port);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static uint _GetPortValueEntityId(IntPtr nodePtr, int port);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static string _GetPortValueString(IntPtr nodePtr, int port);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static bool _GetPortValueBool(IntPtr nodePtr, int port);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static Vec3 _GetPortValueVec3(IntPtr nodePtr, int port);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern internal static IntPtr _GetTargetEntity(IntPtr nodePtr, out uint entId);

        public void RegisterNode(string typeName)
        {
            _RegisterNode(typeName);
        }
        
        public IntPtr GetNode(uint graphId, ushort nodeId)
        {
            return _GetNode(graphId, nodeId);
        }

        public void SetRegularlyUpdated(IntPtr nodePtr, bool updated)
        {
            _SetRegularlyUpdated(nodePtr, updated);
        }

        public bool IsPortActive(IntPtr nodePtr, int port)
        {
            return _IsPortActive(nodePtr, port);
        }

        public void ActivateOutput(IntPtr nodePtr, int port)
        {
            _ActivateOutput(nodePtr,port);
        }

        public void ActivateOutputInt(IntPtr nodePtr, int port, int value)
        {
            _ActivateOutputInt(nodePtr,port,value);
        }

        public void ActivateOutputFloat(IntPtr nodePtr, int port, float value)
        {
           _ActivateOutputFloat(nodePtr,port,value);
        }

        public void ActivateOutputEntityId(IntPtr nodePtr, int port, uint value)
        {
            _ActivateOutputEntityId(nodePtr, port, value);
        }

        public void ActivateOutputString(IntPtr nodePtr, int port, string value)
        {
            _ActivateOutputString(nodePtr, port, value);
        }

        public void ActivateOutputBool(IntPtr nodePtr, int port, bool value)
        {
            _ActivateOutputBool(nodePtr, port,value);
        }

        public void ActivateOutputVec3(IntPtr nodePtr, int port, Vec3 value)
        {
            _ActivateOutputVec3(nodePtr, port,value);
        }

        public int GetPortValueInt(IntPtr nodePtr, int port)
        {
            return _GetPortValueInt(nodePtr, port);
        }

        public float GetPortValueFloat(IntPtr nodePtr, int port)
        {
            return _GetPortValueFloat(nodePtr, port);
        }

        public uint GetPortValueEntityId(IntPtr nodePtr, int port)
        {
            return _GetPortValueEntityId(nodePtr, port);
        }

        public string GetPortValueString(IntPtr nodePtr, int port)
        {
            return _GetPortValueString(nodePtr, port);
        }

        public bool GetPortValueBool(IntPtr nodePtr, int port)
        {
            return _GetPortValueBool(nodePtr, port);
        }

        public Vec3 GetPortValueVec3(IntPtr nodePtr, int port)
        {
            return _GetPortValueVec3(nodePtr, port);
        }

        public IntPtr GetTargetEntity(IntPtr nodePtr, out uint entId)
        {
            return _GetTargetEntity(nodePtr, out entId);
        }
    }
}
