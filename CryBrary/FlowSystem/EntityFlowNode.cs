using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using CryEngine;
using CryEngine.Initialization;
using CryEngine.FlowSystem.Native;

namespace CryEngine.FlowSystem
{
    public abstract class EntityFlowNode<T> : FlowNode where T : EntityBase
    {
        internal override NodeConfig GetNodeConfig()
        {
            var registrationParams = (EntityFlowNodeRegistrationParams)Script.RegistrationParams;

            return new NodeConfig(FlowNodeFilter.Approved, "", FlowNodeFlags.HideUI, FlowNodeType.Instanced, registrationParams.InputPorts, registrationParams.OutputPorts);
        }

        internal void InternalSetTargetEntity(IntPtr handle, EntityId entId)
        {
            targetEntity = Entity.CreateNativeEntity(entId, handle);
        }

        EntityBase targetEntity;
        public override EntityBase TargetEntity { get { return targetEntity; } }
    }
}