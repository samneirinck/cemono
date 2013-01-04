using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using CryEngine.Extensions;
using CryEngine.Initialization;
using CryEngine.Native;

using CryEngine.FlowSystem.Native;

namespace CryEngine.FlowSystem
{
    public abstract class FlowNode : CryScriptInstance
    {
        internal static void Register(string typeName)
        {
            NativeFlowNodeMethods.RegisterNode(typeName);
        }

        internal bool InternalInitialize(NodeInfo nodeInfo)
        {
            NodeHandle = nodeInfo.nodePtr;
            NodeId = nodeInfo.nodeId;
            GraphId = nodeInfo.graphId;

            var registrationParams = (FlowNodeBaseRegistrationParams)Script.RegistrationParams;

            // create instances of OutputPort's.
            for(int i = 0; i < registrationParams.OutputMembers.Length; i++)
            {
                var outputMember = registrationParams.OutputMembers[i];

                Type type;
                if (outputMember.MemberType == MemberTypes.Field)
                    type = (outputMember as FieldInfo).FieldType;
                else
                    type = (outputMember as PropertyInfo).PropertyType;

                bool isGenericType = type.IsGenericType;
                Type genericType = isGenericType ? type.GetGenericArguments()[0] : typeof(void);

                object[] outputPortConstructorArgs = { NodeHandle, i };
                Type genericOutputPort = typeof(OutputPort<>);
                object outputPort = Activator.CreateInstance(isGenericType ? genericOutputPort.MakeGenericType(genericType) : type, outputPortConstructorArgs);

                if (outputMember.MemberType == MemberTypes.Field)
                    (outputMember as FieldInfo).SetValue(this, outputPort);
                else
                    (outputMember as PropertyInfo).SetValue(this, outputPort, null);
            }

            return true;
        }

        internal virtual NodeConfig GetNodeConfig()
        {
            var registrationParams = (FlowNodeRegistrationParams)Script.RegistrationParams;

            return new NodeConfig(registrationParams.filter, registrationParams.description, (registrationParams.hasTargetEntity ? FlowNodeFlags.TargetEntity : 0), registrationParams.type, registrationParams.InputPorts, registrationParams.OutputPorts);
        }

        int GetInputPortId(MethodInfo method)
        {
            var registrationParams = (FlowNodeBaseRegistrationParams)Script.RegistrationParams;

            for (int i = 0; i < registrationParams.InputMethods.Length; i++)
            {
                if (registrationParams.InputMethods[i] == method)
                    return i;
            }

            throw new ArgumentException("Invalid input method specified");
        }

        #region Callbacks
        /// <summary>
        /// Called if one or more input ports have been activated.
        /// </summary>
        internal void OnPortActivated(int index, object value = null)
        {
            var registrationParams = (FlowNodeBaseRegistrationParams)Script.RegistrationParams;

            var method = registrationParams.InputMethods[index];

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
        /// Note that this is called prior to GameRules.OnClientConnect and OnClientEnteredGame!
        /// </summary>
        protected virtual void OnInit() { }
        #endregion

        #region External methods
        protected bool IsPortActive<T>(Action<T> port)
        {
            return NativeFlowNodeMethods.IsPortActive(NodeHandle, GetInputPortId(port.Method));
        }

        protected T GetPortValue<T>(Action<T> port)
        {
            var type = typeof(T);

            if (type == typeof(int))
                return (T)(object)NativeFlowNodeMethods.GetPortValueInt(NodeHandle, GetInputPortId(port.Method));
            if (type == typeof(float))
                return (T)(object)NativeFlowNodeMethods.GetPortValueFloat(NodeHandle, GetInputPortId(port.Method));
            if (type == typeof(Vec3) || type == typeof(Color))
                return (T)(object)NativeFlowNodeMethods.GetPortValueVec3(NodeHandle, GetInputPortId(port.Method));
            if (type == typeof(string))
                return (T)(object)NativeFlowNodeMethods.GetPortValueString(NodeHandle, GetInputPortId(port.Method));
            if (type == typeof(bool))
                return (T)(object)NativeFlowNodeMethods.GetPortValueBool(NodeHandle, GetInputPortId(port.Method));
            if (type == typeof(EntityId))
                return (T)(object)NativeFlowNodeMethods.GetPortValueEntityId(NodeHandle, GetInputPortId(port.Method));
            if (type.IsEnum)
                return (T)Enum.ToObject(typeof(T), NativeFlowNodeMethods.GetPortValueInt(NodeHandle, GetInputPortId(port.Method)));

            throw new ArgumentException("Invalid flownode port type specified!");
        }
        #endregion

        internal void InternalSetTargetEntity(IntPtr handle, EntityId entId)
        {
            TargetEntity = Entity.CreateNativeEntity(entId, handle);
        }

        public EntityBase TargetEntity { get; private set; }

        #region Overrides
        public override int GetHashCode()
        {
            unchecked // Overflow is fine, just wrap
            {
                int hash = 17;

                hash = hash * 29 + ScriptId.GetHashCode();
                hash = hash * 29 + NodeHandle.GetHashCode();
                hash = hash * 29 + NodeId.GetHashCode();
                hash = hash * 29 + GraphId.GetHashCode();

                return hash;
            }
        }
        #endregion

        public IntPtr NodeHandle { get; set; }

        public Int32 NodeId { get; set; }
        public Int64 GraphId { get; set; }
    }
}
