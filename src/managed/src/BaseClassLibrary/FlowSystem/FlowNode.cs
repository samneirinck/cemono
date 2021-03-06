﻿namespace CryEngine.FlowSystem
{
    public class FlowNode
    {
        public struct SActivationInfo
        {
            SActivationInfo(ushort myID = 0)
            {
                this.myID = myID;

                entityId = 0;
                connectPort = 0;
            }

            ushort myID;
            uint entityId;
            byte connectPort;
        };

        public enum EFlowEvent
        {
            Update,
            Activate,				 // Sent if one or more input ports have been activated
            FinalActivate,   // must be eFE_Activate+1 (same as activate, but at the end of FlowSystem:Update)
            Initialize,      // Sent once after level has been loaded. Is NOT called on Serialization!
            FinalInitialize, // must be eFE_Initialize+1
            SetEntityId,     // This event is send to set the entity of the FlowNode. Might also be sent in conjunction (pre) other events (like eFE_Initialize)
            Suspend,
            Resume,
            ConnectInputPort,
            DisconnectInputPort,
            ConnectOutputPort,
            DisconnectOutputPort,
            DontDoAnythingWithThisPlease
        };
    }
}
