using System;

namespace CryEngine.Native
{
    internal interface INativeFlowNodeMethods
    {
        void RegisterNode(string typeName);
       IntPtr GetNode(UInt32 graphId, UInt16 nodeId);

       void SetRegularlyUpdated(IntPtr nodePtr, bool updated);

       bool IsPortActive(IntPtr nodePtr, int port);

       void ActivateOutput(IntPtr nodePtr, int port);
       void ActivateOutputInt(IntPtr nodePtr, int port, int value);
       void ActivateOutputFloat(IntPtr nodePtr, int port, float value);
       void ActivateOutputEntityId(IntPtr nodePtr, int port, uint value);
       void ActivateOutputString(IntPtr nodePtr, int port, string value);
       void ActivateOutputBool(IntPtr nodePtr, int port, bool value);
       void ActivateOutputVec3(IntPtr nodePtr, int port, Vec3 value);

       int GetPortValueInt(IntPtr nodePtr, int port);
       float GetPortValueFloat(IntPtr nodePtr, int port);
       uint GetPortValueEntityId(IntPtr nodePtr, int port);
       string GetPortValueString(IntPtr nodePtr, int port);
       bool GetPortValueBool(IntPtr nodePtr, int port);
       Vec3 GetPortValueVec3(IntPtr nodePtr, int port);

       IntPtr GetTargetEntity(IntPtr nodePtr, out uint entId);

    }
}