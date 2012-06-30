using System;

namespace CryEngine
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
            FlowNode.NativeFlowNodeMethods.ActivateOutput(ParentNodePointer, PortId);
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
			if(value is int)
                FlowNode.NativeFlowNodeMethods.ActivateOutputInt(ParentNodePointer, PortId, System.Convert.ToInt32(value));
			else if(value is float || value is double)
                FlowNode.NativeFlowNodeMethods.ActivateOutputFloat(ParentNodePointer, PortId, System.Convert.ToSingle(value));
			else if(value is EntityId)
                FlowNode.NativeFlowNodeMethods.ActivateOutputEntityId(ParentNodePointer, PortId, (uint)((EntityId)(object)value)._value);
			else if(value is string)
                FlowNode.NativeFlowNodeMethods.ActivateOutputString(ParentNodePointer, PortId, System.Convert.ToString(value));
			else if(value is bool)
                FlowNode.NativeFlowNodeMethods.ActivateOutputBool(ParentNodePointer, PortId, System.Convert.ToBoolean(value));
			else if(value is Vec3)
                FlowNode.NativeFlowNodeMethods.ActivateOutputVec3(ParentNodePointer, PortId, (Vec3)(object)value);
			else
				throw new ArgumentException("Attempted to activate output with invalid value!");
		}

		IntPtr ParentNodePointer { get; set; }
		int PortId { get; set; }
	}

	public enum NodePortType
	{
		Any = -1,
		Void,
		Int,
		Float,
		EntityId,
		Vec3,
		String,
		Bool
	};

	public struct InputPortConfig
	{
		public InputPortConfig(string _name, NodePortType _type, string desc = "", string _humanName = "", string UIConfig = "")
			: this()
		{
			name = _name;
			humanName = _humanName;

			description = desc;
			uiConfig = UIConfig;

			type = _type;

			defaultValue = null;
		}

		public InputPortConfig(string _name, NodePortType _type, object defaultVal = null, string desc = "", string _humanName = "", string UIConfig = "")
			: this(_name, _type, desc, _humanName, UIConfig)
		{
			defaultValue = defaultVal;
		}

		public string name;
		public string humanName;
		public string description;

		public NodePortType type;

		public string uiConfig;

		public object defaultValue;
	}

	public struct OutputPortConfig
	{
		public OutputPortConfig(string _name, string _humanName, string desc, NodePortType _type)
			: this()
		{
			name = _name;
			humanName = _humanName;
			description = desc;
			type = _type;
		}

		public string name;
		public string humanName;
		public string description;

		public NodePortType type;
	}

	public struct NodePortConfig
	{
		public NodePortConfig(object[] inputPorts, object[] outputPorts)
			: this()
		{
			inputs = inputPorts;
			outputs = outputPorts;
		}

		public object[] inputs;
		public object[] outputs;
	}
}