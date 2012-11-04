using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using CryEngine.Extensions;
using CryEngine.Initialization;
using CryEngine.Native;

namespace CryEngine
{
    public abstract class FlowNode : CryScriptInstance
    {
        internal static void Register(string typeName)
        {
            NativeMethods.FlowNode.RegisterNode(typeName);
        }

        internal void InternalInitialize(NodeInfo nodeInfo)
        {
            HandleRef = new HandleRef(this, nodeInfo.nodePtr);
            NodeId = nodeInfo.nodeId;
            GraphId = nodeInfo.graphId;

            var emptyList = new List<object>();
            foreach (var member in GetType().GetMembers().Where(x => x.MemberType == MemberTypes.Field || x.MemberType == MemberTypes.Property))
                ProcessMemberForPort(member, ref emptyList, ref emptyList);
        }

        internal virtual NodeConfig GetNodeConfig()
        {
            var nodeInfo = GetType().GetAttribute<FlowNodeAttribute>();
            return new NodeConfig(nodeInfo.Category != 0 ? nodeInfo.Category : FlowNodeCategory.Approved, nodeInfo.Description, nodeInfo.HasTargetEntity ? FlowNodeFlags.TargetEntity : 0);
        }

        internal virtual NodePortConfig GetPortConfig()
        {
            if (InputMethods == null)
                InputMethods = new Dictionary<Type, List<MethodInfo>>();

            Type type = GetType();
            if (!InputMethods.ContainsKey(type))
                InputMethods.Add(type, new List<MethodInfo>());

            InputMethods[type].Clear();

            var inputs = new List<object>();
            var outputs = new List<object>();

            foreach (var member in type.GetMembers())
                ProcessMemberForPort(member, ref inputs, ref outputs);

            return new NodePortConfig(inputs.ToArray(), outputs.ToArray());
        }

        void ProcessMemberForPort(MemberInfo member, ref List<object> inputs, ref List<object> outputs)
        {
            PortAttribute portAttribute;
            if (member.TryGetAttribute(out portAttribute))
            {
                switch (member.MemberType)
                {
                    case MemberTypes.Method:
                        ProcessInputPort(portAttribute, member as MethodInfo, ref inputs);
                        break;
                    case MemberTypes.Field:
                        ProcessOutputPort(portAttribute, member as FieldInfo, ref outputs);
                        break;
                    case MemberTypes.Property:
                        ProcessOutputPort(portAttribute, member as PropertyInfo, ref outputs);
                        break;
                }
            }
        }

        void ProcessInputPort(PortAttribute portAttribute, MethodInfo method, ref List<object> inputs)
        {
            NodePortType portType;
            object defaultVal = null;

            if (method.GetParameters().Length > 0)
            {
                ParameterInfo parameter = method.GetParameters()[0];
                if (parameter.ParameterType.IsEnum)
                {
                    portType = NodePortType.Int;

                    var values = Enum.GetValues(parameter.ParameterType);
                    if (values.Length <= 0)
                        return;

                    defaultVal = values.GetValue(0);

                    portAttribute.UIConfig = "enum_int:";

                    for (int i = 0; i < values.Length; i++)
                    {
                        var value = values.GetValue(i);

                        if (i > 0 && i != values.Length)
                            portAttribute.UIConfig += ",";

                        portAttribute.UIConfig += Enum.GetName(parameter.ParameterType, value) + "=" + (int)value;
                    }
                }
                else
                    portType = GetPortType(parameter.ParameterType);

                if (parameter.IsOptional && defaultVal == null)
                    defaultVal = parameter.DefaultValue;
                else if (defaultVal == null)
                {
                    switch (portType)
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

            string inputPortType = "";
            switch (portAttribute.Type)
            {
                case PortType.Sound:
                    {
                        if (portType == NodePortType.String)
                            inputPortType = "sound_";
                    }
                    break;
                case PortType.DialogLine:
                    {
                        if (portType == NodePortType.String)
                            inputPortType = "dialogline_";
                    }
                    break;
                case PortType.Color:
                    {
                        if (portType == NodePortType.Vec3)
                            inputPortType = "color_";
                    }
                    break;
                case PortType.Texture:
                    {
                        if (portType == NodePortType.String)
                            inputPortType = "texture_";
                    }
                    break;
                case PortType.Object:
                    {
                        if (portType == NodePortType.String)
                            inputPortType = "object_";
                    }
                    break;
                case PortType.File:
                    {
                        if (portType == NodePortType.String)
                            inputPortType = "file_";
                    }
                    break;
                case PortType.EquipmentPack:
                    {
                        if (portType == NodePortType.String)
                            inputPortType = "equip_";
                    }
                    break;
                case PortType.ReverbPreset:
                    {
                        if (portType == NodePortType.String)
                            inputPortType = "reverbpreset_";
                    }
                    break;
                case PortType.GameToken:
                    {
                        if (portType == NodePortType.String)
                            inputPortType = "gametoken_";
                    }
                    break;
                case PortType.Material:
                    {
                        if (portType == NodePortType.String)
                            inputPortType = "mat_";
                    }
                    break;
                case PortType.Sequence:
                    {
                        if (portType == NodePortType.String)
                            inputPortType = "seq_";
                    }
                    break;
                case PortType.Mission:
                    {
                        if (portType == NodePortType.String)
                            inputPortType = "mission_";
                    }
                    break;
                case PortType.Animation:
                    {
                        if (portType == NodePortType.String)
                            inputPortType = "anim_";
                    }
                    break;
                case PortType.AnimationState:
                    {
                        if (portType == NodePortType.String)
                            inputPortType = "animstate_";
                    }
                    break;
                case PortType.AnimationStateEx:
                    {
                        if (portType == NodePortType.String)
                            inputPortType = "animstateEx_";
                    }
                    break;
                case PortType.Bone:
                    {
                        if (portType == NodePortType.String)
                            inputPortType = "bone_";
                    }
                    break;
                case PortType.Attachment:
                    {
                        if (portType == NodePortType.String)
                            inputPortType = "attachment_";
                    }
                    break;
                case PortType.Dialog:
                    {
                        if (portType == NodePortType.String)
                            inputPortType = "dialog_";
                    }
                    break;
                case PortType.MaterialParamSlot:
                    {
                        if (portType == NodePortType.String)
                            inputPortType = "matparamslot_";
                    }
                    break;
                case PortType.MaterialParamName:
                    {
                        if (portType == NodePortType.String)
                            inputPortType = "matparamname_";
                    }
                    break;
                case PortType.MaterialParamCharacterAttachment:
                    {
                        if (portType == NodePortType.String)
                            inputPortType = "matparamcharatt_";
                    }
                    break;
            }

            string name = inputPortType + (portAttribute.Name ?? method.Name);

            inputs.Add(new InputPortConfig(name, portType, defaultVal, portAttribute.Description, portAttribute.Name ?? method.Name, portAttribute.UIConfig));
            InputMethods[GetType()].Add(method);
        }

        void ProcessOutputPort(PortAttribute portAttribute, FieldInfo field, ref List<object> outputs)
        {
            if (portAttribute.Name == null)
                portAttribute.Name = field.Name;

            field.SetValue(this, ProcessOutputPortCommon(portAttribute, field.FieldType, ref outputs));
        }

        void ProcessOutputPort(PortAttribute portAttribute, PropertyInfo property, ref List<object> outputs)
        {
            if (portAttribute.Name == null)
                portAttribute.Name = property.Name;

            property.SetValue(this, ProcessOutputPortCommon(portAttribute, property.PropertyType, ref outputs), null);
        }

        object ProcessOutputPortCommon(PortAttribute portAttribute, Type type, ref List<object> outputs)
        {
            if (type.Name.StartsWith("OutputPort"))
            {
                bool isGenericType = type.IsGenericType;
                Type genericType = isGenericType ? type.GetGenericArguments()[0] : typeof(void);

                var portType = GetPortType(genericType);

                object[] outputPortConstructorArgs = { HandleRef.Handle, outputs.Count };
                Type genericOutputPort = typeof(OutputPort<>);
                object outputPort = Activator.CreateInstance(isGenericType ? genericOutputPort.MakeGenericType(genericType) : type, outputPortConstructorArgs);

                var portConfig = new OutputPortConfig(portAttribute.Name, portAttribute.Name, portAttribute.Description, portType);

                if (!outputs.Contains(portConfig))
                    outputs.Add(portConfig);

                return outputPort;
            }

            return null;
        }

        // Used to call OnActivate methods automatically.
        static Dictionary<Type, List<MethodInfo>> InputMethods { get; set; }

        // DON'T LOOK AT ME
        internal static NodePortType GetPortType(Type type)
        {
            if (type == typeof(void))
                return NodePortType.Void;
            if (type == typeof(int))
                return NodePortType.Int;
            if (type == typeof(float))
                return NodePortType.Float;
            if (type == typeof(string))
                return NodePortType.String;
            if (type == typeof(Vec3))
                return NodePortType.Vec3;
            if (type == typeof(bool))
                return NodePortType.Bool;
            if (type == typeof(EntityId))
                return NodePortType.EntityId;
            
            throw new ArgumentException("Invalid flownode port type specified!");
        }

        #region Callbacks
        /// <summary>
        /// Called if one or more input ports have been activated.
        /// </summary>
        internal void OnPortActivated(int index, object value = null)
        {
#if !(RELEASE && RELEASE_DISABLE_CHECKS)
            if (InputMethods == null)
                throw new Exception("InputMethods was null!");
            if (!InputMethods.ContainsKey(GetType()))
                throw new Exception("InputMethods did not contain the flownode type!");
#endif

            var method = InputMethods[GetType()].ElementAt(index);

            if (value != null && method.GetParameters().Length > 0)
            {
                var parameters = new[] { value };
                method.Invoke(this, parameters);
            }
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
            return NativeMethods.FlowNode.GetPortValueInt(HandleRef.Handle, GetInputPortId(port.Method));
        }

        protected bool IsIntPortActive(Action<int> port)
        {
            return IsPortActive(GetInputPortId(port.Method));
        }

        protected T GetPortEnum<T>(Action<T> port) where T : struct
        {
#if !(RELEASE && RELEASE_DISABLE_CHECKS)
            if (!typeof(T).IsEnum)
                throw new ArgumentException("T must be an enumerated type");
#endif

            return (T)Enum.ToObject(typeof(T), NativeMethods.FlowNode.GetPortValueInt(HandleRef.Handle, GetInputPortId(port.Method)));
        }

        protected bool IsEnumPortActive(Action<Enum> port)
        {
            return IsPortActive(GetInputPortId(port.Method));
        }

        /// <summary>
        /// Gets the float value of an flownode port.
        /// </summary>
        /// <param name="port"></param>
        /// <returns></returns>
        protected float GetPortFloat(Action<float> port)
        {
            return NativeMethods.FlowNode.GetPortValueFloat(HandleRef.Handle, GetInputPortId(port.Method));
        }

        protected bool IsFloatPortActive(Action<float> port)
        {
            return IsPortActive(GetInputPortId(port.Method));
        }

        /// <summary>
        /// Gets the int value of an flownode port.
        /// </summary>
        /// <param name="port"></param>
        /// <returns></returns>
        protected Vec3 GetPortVec3(Action<Vec3> port)
        {
            return NativeMethods.FlowNode.GetPortValueVec3(HandleRef.Handle, GetInputPortId(port.Method));
        }

        protected bool IsVec3PortActive(Action<Vec3> port)
        {
            return IsPortActive(GetInputPortId(port.Method));
        }

        /// <summary>
        /// Gets the string value of an flownode port.
        /// </summary>
        /// <param name="port"></param>
        /// <returns></returns>
        protected string GetPortString(Action<string> port)
        {
            return NativeMethods.FlowNode.GetPortValueString(HandleRef.Handle, GetInputPortId(port.Method));
        }

        protected bool IsStringPortActive(Action<string> port)
        {
            return IsPortActive(GetInputPortId(port.Method));
        }

        /// <summary>
        /// Gets the bool value of an flownode port.
        /// </summary>
        /// <param name="port"></param>
        /// <returns></returns>
        protected bool GetPortBool(Action<bool> port)
        {
            return NativeMethods.FlowNode.GetPortValueBool(HandleRef.Handle, GetInputPortId(port.Method));
        }

        protected bool IsBoolPortActive(Action<bool> port)
        {
            return IsPortActive(GetInputPortId(port.Method));
        }

        protected EntityId GetPortEntityId(Action<EntityId> port)
        {
            return new EntityId(NativeMethods.FlowNode.GetPortValueEntityId(HandleRef.Handle, GetInputPortId(port.Method)));
        }

        protected bool IsEntityIdPortActive(Action<EntityId> port)
        {
            return IsPortActive(GetInputPortId(port.Method));
        }

        int GetInputPortId(MethodInfo method)
        {
            var methods = InputMethods[GetType()];

            var index = methods.IndexOf(method);
            if (index != -1)
                return index;

            throw new ArgumentException("Invalid input method specified");
        }

        /// <summary>
        /// Used to check whether an input port is currently activated.
        /// </summary>
        /// <param name="port"></param>
        /// <returns></returns>
        private bool IsPortActive(int port) { return NativeMethods.FlowNode.IsPortActive(HandleRef.Handle, port); }
        #endregion

        public EntityBase TargetEntity
        {
            get
            {
                uint entId;
                var entPtr = NativeMethods.FlowNode.GetTargetEntity(HandleRef.Handle, out entId);

                if (entPtr != IntPtr.Zero)
                    return Entity.CreateNativeEntity(new EntityId(entId), entPtr);

                return null;
            }
        }

        #region Overrides
        public override int GetHashCode()
        {
            unchecked // Overflow is fine, just wrap
            {
                int hash = 17;

                hash = hash * 29 + ScriptId.GetHashCode();
                hash = hash * 29 + HandleRef.GetHashCode();
                hash = hash * 29 + NodeId.GetHashCode();
                hash = hash * 29 + GraphId.GetHashCode();

                return hash;
            }
        }
        #endregion

        public HandleRef HandleRef { get; set; }

        internal UInt16 NodeId { get; set; }

        internal UInt32 GraphId { get; set; }

        internal bool Initialized { get; set; }
    }

    struct NodeInfo
    {
        public NodeInfo(IntPtr Pointer, UInt16 NodeId, UInt32 GraphId)
        {
            nodePtr = Pointer;
            nodeId = NodeId;
            graphId = GraphId;
        }

        public IntPtr nodePtr;

        public UInt16 nodeId;

        public UInt32 graphId;
    }

    internal struct NodeConfig
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
}
