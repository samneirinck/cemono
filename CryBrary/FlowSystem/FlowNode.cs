using System;
using System.Linq;
using System.Collections.Generic;
using System.Reflection;

using System.Runtime.CompilerServices;

using CryEngine.Extensions;

namespace CryEngine
{
	public abstract class FlowNode : CryScriptInstance
	{
		#region Externals
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern internal static void _RegisterNode(string typeName);

		internal static void Register(string typeName)
		{
			_RegisterNode(typeName);
		}

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
		#endregion

		internal void InternalInitialize(NodeInfo nodeInfo)
		{
			NodePointer = nodeInfo.nodePtr;
		}

		internal virtual NodeConfig GetNodeConfig()
		{
			var nodeInfo = GetType().GetAttribute<FlowNodeAttribute>();

			return new NodeConfig(nodeInfo.Category, nodeInfo.Description);
		}

		internal virtual NodePortConfig GetPortConfig()
		{
			if(inputMethods == null)
				inputMethods = new List<MethodInfo>();

			Type type = GetType();

			var inputs = new List<object>();
			var outputs = new List<object>();

			foreach(var member in type.GetMembers())
				ProcessMemberForPort(member, ref inputs, ref outputs);

			return new NodePortConfig(inputs.ToArray(), outputs.ToArray());
		}

		public void ProcessMemberForPort(MemberInfo member, ref List<object> inputs, ref List<object> outputs)
		{
			PortAttribute portAttribute;
			if(member.TryGetAttribute(out portAttribute))
			{
				MethodInfo method = member as MethodInfo;
				if(method != null)
				{
					NodePortType portType;
					object defaultVal = null;

					if(method.GetParameters().Length > 0)
					{
						ParameterInfo parameter = method.GetParameters()[0];
						portType = GetPortType(parameter.ParameterType);

						if(parameter.IsOptional)
							defaultVal = parameter.DefaultValue;
						else
						{
							switch(portType)
							{
								case NodePortType.Bool:
									defaultVal = false;
									break;
								case NodePortType.EntityId:
									defaultVal = 0;
									break;
								case NodePortType.Float:
									defaultVal = 0.0f;
									break;
								case NodePortType.Int:
									defaultVal = 0;
									break;
								case NodePortType.String:
									defaultVal = "";
									break;
								case NodePortType.Vec3:
									defaultVal = Vec3.Zero;
									break;
							}
						}
					}
					else
						portType = NodePortType.Void;

					inputs.Add(new InputPortConfig(portAttribute.Name, portType, defaultVal, portAttribute.Description));
					inputMethods.Add(method);
				}

				FieldInfo field = member as FieldInfo;
				PropertyInfo property = member as PropertyInfo;
				if(field != null || property != null)
				{
					NodePortType portType = 0;

					if(field != null)
					{
						if(field.FieldType.Name.StartsWith("OutputPort"))
						{
							bool isGenericType = field.FieldType.IsGenericType;
							Type genericType = isGenericType ? field.FieldType.GetGenericArguments()[0] : typeof(void);

							portType = GetPortType(genericType);

							object[] outputPortConstructorArgs = { NodePointer, (outputs.Count - 1) };
							Type genericOutputPort = typeof(OutputPort<>);
							object outputPort = Activator.CreateInstance(isGenericType ? genericOutputPort.MakeGenericType(genericType) : field.FieldType, outputPortConstructorArgs);

							field.SetValue(this, outputPort);
						}
					}
					else
					{
						if(property.PropertyType.Name.StartsWith("OutputPort"))
						{
							bool isGenericType = property.PropertyType.IsGenericType;
							Type genericType = isGenericType ? property.PropertyType.GetGenericArguments()[0] : typeof(void);

							portType = GetPortType(genericType);
							outputs.Add(new OutputPortConfig(portAttribute.Name, portAttribute.Name, portAttribute.Description, portType));

							object[] outputPortConstructorArgs = { NodePointer, (outputs.Count - 1) };
							Type genericOutputPort = typeof(OutputPort<>);
							object outputPort = Activator.CreateInstance(isGenericType ? genericOutputPort.MakeGenericType(genericType) : property.PropertyType, outputPortConstructorArgs);

							property.SetValue(this, outputPort, null);
						}
					}

					var portConfig = new OutputPortConfig(portAttribute.Name, portAttribute.Name, portAttribute.Description, portType);

					if(!outputs.Contains(portConfig))
						outputs.Add(portConfig);
				}
			}
		}

		// Used to call OnActivate methods automatically.
		List<MethodInfo> inputMethods;

		//DON'T LOOK AT ME
		static NodePortType GetPortType(Type type)
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
		internal void OnPortActivated(int index, object value = null)
		{
			var method = inputMethods.ElementAt(index);

			if(value != null && method.GetParameters().Length > 0)
				method.Invoke(this, new object[] { value });
			else
				method.Invoke(this, null);
		}

		/// <summary>
		/// Called after level has been loaded, is not called on serialization.
		/// </summary>
		protected virtual void OnInit()
		{
		}
		#endregion

		#region External methods
		/// <summary>
		/// Gets the int value of an flownode port.
		/// </summary>
		/// <param name="port"></param>
		/// <returns></returns>
		protected int GetPortInt(Action<int> port)
		{
			return _GetPortValueInt(NodePointer, GetInputPortId(port.Method));
		}

		/// <summary>
		/// Gets the float value of an flownode port.
		/// </summary>
		/// <param name="port"></param>
		/// <returns></returns>
		protected float GetPortFloat(Action<float> port)
		{
			return _GetPortValueFloat(NodePointer, GetInputPortId(port.Method));
		}

		/// <summary>
		/// Gets the int value of an flownode port.
		/// </summary>
		/// <param name="port"></param>
		/// <returns></returns>
		protected Vec3 GetPortVec3(Action<Vec3> port)
		{
			return _GetPortValueVec3(NodePointer, GetInputPortId(port.Method));
		}

		/// <summary>
		/// Gets the string value of an flownode port.
		/// </summary>
		/// <param name="port"></param>
		/// <returns></returns>
		protected string GetPortString(Action<string> port)
		{
			return _GetPortValueString(NodePointer, GetInputPortId(port.Method));
		}

		/// <summary>
		/// Gets the bool value of an flownode port.
		/// </summary>
		/// <param name="port"></param>
		/// <returns></returns>
		protected bool GetPortBool(Action<bool> port)
		{
			return _GetPortValueBool(NodePointer, GetInputPortId(port.Method));
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
		protected bool IsPortActive(int port) { return _IsPortActive(NodePointer, port); }
		#endregion

		internal IntPtr NodePointer { get; set; }
		internal bool Initialized { get; set; }
	}

	struct NodeInfo
	{
		public IntPtr nodePtr;
	}

	[AttributeUsage(AttributeTargets.Class)]
	public sealed class FlowNodeAttribute : Attribute
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

	[AttributeUsage(AttributeTargets.Method | AttributeTargets.Property | AttributeTargets.Field)]
	public sealed class PortAttribute : Attribute
	{
		public string Name { get; set; }
		public string Description { get; set; }
		public FlowNodeFlags Flags { get; set; }
	}

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
			FlowNode._ActivateOutput(ParentNodePointer, PortId);
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
				FlowNode._ActivateOutputInt(ParentNodePointer, PortId, System.Convert.ToInt32(value));
			else if(value is float || value is double)
				FlowNode._ActivateOutputFloat(ParentNodePointer, PortId, System.Convert.ToSingle(value));
			else if(value is uint)
				FlowNode._ActivateOutputEntityId(ParentNodePointer, PortId, System.Convert.ToUInt32(value));
			else if(value is string)
				FlowNode._ActivateOutputString(ParentNodePointer, PortId, System.Convert.ToString(value));
			else if(value is bool)
				FlowNode._ActivateOutputBool(ParentNodePointer, PortId, System.Convert.ToBoolean(value));
			else if(value is Vec3)
				FlowNode._ActivateOutputVec3(ParentNodePointer, PortId, (Vec3)(object)value);
			else
				throw new ArgumentException("Attempted to activate output with invalid value!");
		}

		IntPtr ParentNodePointer { get; set; }
		int PortId { get; set; }
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
		/// <summary>
		/// This node cannot be deleted by the user.
		/// </summary>
		Unremovable = 0x0008,
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
