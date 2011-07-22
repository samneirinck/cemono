using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CryEngine.FlowSystem
{
    public enum EInputPortType
    {
        Void,
        String,
        Int
    };

    public struct SInputPortConfig
    {
        public SInputPortConfig(string sName, EInputPortType inType, string sDescription = null, string sHumanName = null, string UIConfig = null)
        {
            name = sName;
            description = sDescription;
            humanName = sHumanName;
            sUIConfig = UIConfig;

            inputType = inType;
        }

	    // name of this port
        public string name;
	    // Human readable name of this port (default: same as name)
        public string humanName;
	    // Human readable description of this port (help)
        public string description;
	    // UIConfig: enums for the variable e.g 
	    // "enum_string:a,b,c"
	    // "enum_string:something=a,somethingelse=b,whatever=c"
	    // "enum_int:something=0,somethingelse=10,whatever=20"
	    // "enum_float:something=1.0,somethingelse=2.0"
	    // "enum_global:GlobalEnumName"
        public string sUIConfig;

        public EInputPortType inputType;
    };

    public struct SOutputPortConfig
    {
        public SOutputPortConfig(string sName, string sDescription = null, string sHumanName = null)
        {
            name = sName;
            description = sDescription;
            humanName = sHumanName;
        }

	    // name of this port
	    public string name;
	    // Human readable name of this port (default: same as name)
        public string humanName;
	    // Human readable description of this port (help)
        public string description;
    };

    public enum EFlowNodeCategory
    {
        EFLN_APPROVED							 = 0x0010, // CATEGORY:  This node is approved for designers
	    EFLN_ADVANCED              = 0x0020, // CATEGORY:  This node is slightly advanced and approved
	    EFLN_DEBUG                 = 0x0040, // CATEGORY:  This node is for debug purpose only
	    EFLN_WIP                   = 0x0080, // CATEGORY:  This node is work-in-progress and shouldn't be used by designers
	    EFLN_LEGACY                = 0x0100, // CATEGORY:  This node is legacy and will VERY soon vanish
	    EFLN_OBSOLETE              = 0x0200, // CATEGORY:  This node is obsolete and is not available in the editor
    };

    public struct SFlowNodeConfig
    {
        public bool targetsEntity;

        public SInputPortConfig[] pInputPorts;
        public SOutputPortConfig[] pOutputPorts;

        public string sDescription;
        public string sUIClassName;

        public EFlowNodeCategory category;
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

    public class IFlowNode
    {
        public struct SNodeInfo
        {
            ushort nodeId;
            uint entityId;
        };

        public bool IsPortActive( SNodeInfo nodeInfo,int nPort )
	    {
            // nodeInfo
            // return pActInfo->pInputPorts[nPort].IsUserFlagSet();

            return false;
	    }

        public void ActivateOutput(SNodeInfo nodeInfo, int nPort)
        {
        }
    };
}