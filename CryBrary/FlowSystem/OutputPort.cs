using System;

using CryEngine.Native;

namespace CryEngine//.FlowSystem
{
    public sealed class OutputPort
    {
        public OutputPort() { }

        public OutputPort(IntPtr nodePtr, int portId)
        {
            ParentNodePointer = nodePtr;
            PortId = portId;
        }

        public void Activate()
        {
            NativeFlowNodeMethods.ActivateOutput(ParentNodePointer, PortId);
        }

        IntPtr ParentNodePointer { get; set; }

        int PortId { get; set; }
    }

    public sealed class OutputPort<T>
    {
        public OutputPort() { }

        public OutputPort(IntPtr nodePtr, int portId)
        {
            ParentNodePointer = nodePtr;
            PortId = portId;
        }

        public void Activate(T value)
        {
            if (value is int)
                NativeFlowNodeMethods.ActivateOutputInt(ParentNodePointer, PortId, System.Convert.ToInt32(value));
            else if (value is float || value is double)
                NativeFlowNodeMethods.ActivateOutputFloat(ParentNodePointer, PortId, System.Convert.ToSingle(value));
            else if (value is EntityId)
                NativeFlowNodeMethods.ActivateOutputEntityId(ParentNodePointer, PortId, (uint)((EntityId)(object)value)._value);
            else if (value is string)
                NativeFlowNodeMethods.ActivateOutputString(ParentNodePointer, PortId, System.Convert.ToString(value));
            else if (value is bool)
                NativeFlowNodeMethods.ActivateOutputBool(ParentNodePointer, PortId, System.Convert.ToBoolean(value));
            else if (value is Vec3)
                NativeFlowNodeMethods.ActivateOutputVec3(ParentNodePointer, PortId, (Vec3)(object)value);
            else
                throw new ArgumentException("Attempted to activate output with invalid value!");
        }

        IntPtr ParentNodePointer { get; set; }

        int PortId { get; set; }
    }
}
