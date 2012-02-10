using System;
using System.Collections.Generic;
using System.Reflection;
using CryEngine.Extensions;

namespace CryEngine
{
	public class FlowNode : CryScriptInstance
	{
        public virtual NodeConfig GetNodeConfig()
        {
            var nodeInfo = GetType().GetAttribute<FlowNodeAttribute>();

            return new NodeConfig(nodeInfo.Category, nodeInfo.Description);
        }

        public NodePortConfig GetPortConfig()
        {
            if (inputMethods == null)
                inputMethods = new List<MethodInfo>();

            Type type = GetType();

            var inputs = new List<object>();
            var outputs = new List<object>();
            object defaultVal = null;
            //CryConsole.LogAlways("Registering node {0}, category {1}", GetType().Name, nodeInfo.Category);
            foreach (var method in type.GetMethods())
            {
                InputPortAttribute input;
                NodePortType portType;

                if (method.TryGetAttribute(out input))
                {
                    if (method.GetParameters().Length > 0)
                    {
                        ParameterInfo parameter = method.GetParameters()[0];
                        portType = GetPortType(parameter.ParameterType);

                        if (parameter.IsOptional)
                            defaultVal = parameter.DefaultValue;
                        else
                        {
                            switch (portType)
                            {
                                case NodePortType.Bool:
                                    {
                                        defaultVal = false;
                                    }
                                    break;
                                case NodePortType.EntityId:
                                    {
                                        defaultVal = -1;
                                    }
                                    break;
                                case NodePortType.Float:
                                    {
                                        defaultVal = 0.0f;
                                    }
                                    break;
                                case NodePortType.Int:
                                    {
                                        defaultVal = 0;
                                    }
                                    break;
                                case NodePortType.String:
                                    {
                                        defaultVal = "";
                                    }
                                    break;
                                case NodePortType.Vec3:
                                    {
                                        defaultVal = new Vec3(0, 0, 0);
                                    }
                                    break;
                            }
                        }
                    }
                    else
                        portType = NodePortType.Void;


                    inputs.Add(new InputPortConfig(input.Name, portType, defaultVal, input.Description));
                    inputMethods.Add(method);
                }
            }

            OutputPortAttribute output;
            foreach (var property in type.GetProperties())
            {
                if (property.TryGetAttribute(out output))
                {
                    outputs.Add(new OutputPortConfig(output.Name, output.Name, output.Description, output.Type));

                    // outputId, used together with ActivateOutput.
                    property.SetValue(this, (outputs.Count - 1), null);
                }
            }
            foreach (var field in type.GetFields())
            {
                if (field.TryGetAttribute(out output))
                {
                    outputs.Add(new OutputPortConfig(output.Name, output.Name, output.Description, output.Type));

                    // outputId, used together with ActivateOutput.
                    field.SetValue(this, (outputs.Count - 1));
                }
            }

            return new NodePortConfig(inputs.ToArray(), outputs.ToArray());
        }

		// Used to call OnActivate methods automatically.
		List<MethodInfo> inputMethods;

		//DON'T LOOK AT ME
		NodePortType GetPortType(Type type)
		{
			if(type == typeof(void))
				return NodePortType.Void;
			else if(type == typeof(int))
				return NodePortType.Int;
			else if(type == typeof(float))
				return NodePortType.Float;
			else if(type == typeof(string))
				return NodePortType.String;
			else if(type == typeof(Vec3))
				return NodePortType.Vec3;
			else if(type == typeof(bool))
				return NodePortType.Bool;
			else if(type == typeof(uint))
				return NodePortType.EntityId;
			else
				throw new ArgumentException("Invalid flownode port type specified!");
		}

		#region Callbacks
		/// <summary>
		/// Called if one or more input ports have been activated.
		/// </summary>
		public void OnPortActivated(int index, object value = null)
		{
			Console.LogAlways("Triggering port {0}", inputMethods[index].Name);

			if(value != null && inputMethods[index].GetParameters().Length > 0)
				inputMethods[index].Invoke(this, new object[] { value });
			else
				inputMethods[index].Invoke(this, null);
		}

		/// <summary>
		/// Called after level has been loaded, is not called on serialization.
		/// </summary>
        public virtual void OnInit()
		{
		}
		#endregion

		#region External methods
		/// <summary>
		/// Activates one of the node's output ports, without an output value. (Used for outputs of type NodePortType.Void)
		/// </summary>
		/// <param name="port"></param>
		public void ActivateOutput(int port) { FlowSystem._ActivateOutput(ScriptId, port); }
		/// <summary>
		/// Activates one of the node's output ports, with the desired output value.
		/// </summary>
		/// <param name="port">The id of the port, from the order it was registered (first is 0)</param>
		/// <param name="value"></param>
		public void ActivateOutput(int port, object value)
		{
			if(value is int)
				FlowSystem._ActivateOutputInt(ScriptId, port, Convert.ToInt32(value));
			else if(value is float || value is double)
				FlowSystem._ActivateOutputFloat(ScriptId, port, Convert.ToSingle(value));
			else if(value is uint)
				FlowSystem._ActivateOutputEntityId(ScriptId, port, Convert.ToUInt32(value));
			else if(value is string)
				FlowSystem._ActivateOutputString(ScriptId, port, Convert.ToString(value));
			else if(value is bool)
				FlowSystem._ActivateOutputBool(ScriptId, port, Convert.ToBoolean(value));
            else if (value is Vec3)
                FlowSystem._ActivateOutputVec3(ScriptId, port, (Vec3)value);
            else
                throw new ArgumentException("Attempted to activate output with invalid value!");
		}

		public int GetIntValue(Action<int> port)
		{
			return FlowSystem._GetPortValueInt(ScriptId, GetInputPortId(port.Method));
		}

		public float GetFloatValue(Action<float> port)
		{
			return FlowSystem._GetPortValueFloat(ScriptId, GetInputPortId(port.Method));
		}

		public Vec3 GetVec3Value(Action<Vec3> port)
		{
			return FlowSystem._GetPortValueVec3(ScriptId, GetInputPortId(port.Method));
		}

		public string GetStringValue(Action<string> port)
		{
			return FlowSystem._GetPortValueString(ScriptId, GetInputPortId(port.Method));
		}

		public bool GetBoolValue(Action<bool> port)
		{
			return FlowSystem._GetPortValueBool(ScriptId, GetInputPortId(port.Method));
		}

		int GetInputPortId(MethodInfo method)
		{
			for(int i = 0; i < inputMethods.Count; i++)
			{
				if(method == inputMethods[i])
					return i;
			}

			throw new ArgumentException("Invalid input method specified");
		}

		/// <summary>
		/// Used to check whether an input port is currently activated.
		/// </summary>
		/// <param name="port"></param>
		/// <returns></returns>
		public bool IsPortActive(int port) { return FlowSystem._IsPortActive(ScriptId, port); }
		#endregion

		internal bool Initialized;
	}

	[AttributeUsage(AttributeTargets.Class)]
	public class FlowNodeAttribute : Attribute
	{
        /// <summary>
        /// Name of the node, if not set will use the node class name.
        /// </summary>
        public string Name { get; set; }
		/// <summary>
		/// Category in which the node will appear when right-clicking in the Flowgraph Editor.
		/// </summary>
		public string UICategory { get; set; }
		/// <summary>
		/// The Sandbox filtering category
		/// </summary>
		public FlowNodeCategory Category { get; set; }
		public string Description { get; set; }
		public FlowNodeFlags Flags { get; set; }
	}

	[AttributeUsage(AttributeTargets.Method)]
	public class InputPortAttribute : Attribute
	{
		public string Name { get; set; }
		public string Description { get; set; }
		public FlowNodeFlags Flags { get; set; }
	}

	[AttributeUsage(AttributeTargets.Property | AttributeTargets.Field)]
	public class OutputPortAttribute : Attribute
	{
		public string Name { get; set; }
		public string Description { get; set; }
		public NodePortType Type { get; set; }
		public FlowNodeFlags Flags { get; set; }
	}

    [Flags]
	public enum FlowNodeFlags
	{
		/// <summary>
		/// This node targets an entity, entity id must be provided.
		/// </summary>
		TargetEntity = 0x0001,
		/// <summary>
		/// This node cannot be selected by user for placement in flow graph UI.
		/// </summary>
		HideUI = 0x0002,
		/// <summary>
		/// This node is setup for dynamic output port growth in runtime.
		/// </summary>
		DynamicOutput = 0x0004,
	}

	public enum FlowNodeCategory
	{
		/// <summary>
		/// This node is approved for designers
		/// </summary>
		Approved = 0x0010,
		/// <summary>
		/// This node is slightly advanced and approved.
		/// </summary>
		Advanced = 0x0020,
		/// <summary>
		/// This node is for debug purpose only.
		/// </summary>
		Debug = 0x0040,
		/// <summary>
		/// This node is obsolete and is not available in the editor.
		/// </summary>
		Obsolete = 0x0200,
	};

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

	public struct NodeConfig
	{
		public NodeConfig(FlowNodeCategory cat, string desc, FlowNodeFlags nodeFlags = 0)
			: this()
		{
			flags = nodeFlags;
			category = cat;
			description = desc;
		}

		FlowNodeFlags flags;
		FlowNodeCategory category;
		string description;
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
