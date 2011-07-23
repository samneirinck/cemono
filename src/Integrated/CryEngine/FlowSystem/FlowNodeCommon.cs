using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CryEngine.FlowSystem
{
    public class CG2AutoRegFlowNode<T> : CG2AutoRegFlowNodeBase
    {
        public CG2AutoRegFlowNode(string nodeClassName) : base(nodeClassName)
        {
        }
    }

    public class CG2AutoRegFlowNodeBase
    {
        public CG2AutoRegFlowNodeBase(string nodeClassName)
        {
            m_sClassName = nodeClassName;
            m_pNext = null;
            if (m_pLast == null)
            {
                m_pFirst = this;
            }
            else
                m_pLast.m_pNext = this;
            m_pLast = this;
        }

        public CG2AutoRegFlowNodeBase()
        {
        }

        //////////////////////////////////////////////////////////////////////////
        string m_sClassName;
        public CG2AutoRegFlowNodeBase m_pNext;
        public static CG2AutoRegFlowNodeBase m_pFirst;
        public static CG2AutoRegFlowNodeBase m_pLast;
        //////////////////////////////////////////////////////////////////////////
    }

    // Registration table
    public struct SPluginRegister
    {
        public SPluginRegister(bool useless)
        {
            nodesFirst = null;
            nodesLast = null;
        }

	    // Flownode registration list
	    public CG2AutoRegFlowNodeBase nodesFirst;
	    public CG2AutoRegFlowNodeBase nodesLast;
    }

    public enum EInputPortType
    {
        Void,
        String,
        Int
    }

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
    }

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
    }

    public enum EFlowNodeCategory
    {
        EFLN_APPROVED							 = 0x0010, // CATEGORY:  This node is approved for designers
	    EFLN_ADVANCED              = 0x0020, // CATEGORY:  This node is slightly advanced and approved
	    EFLN_DEBUG                 = 0x0040, // CATEGORY:  This node is for debug purpose only
	    EFLN_WIP                   = 0x0080, // CATEGORY:  This node is work-in-progress and shouldn't be used by designers
	    EFLN_LEGACY                = 0x0100, // CATEGORY:  This node is legacy and will VERY soon vanish
	    EFLN_OBSOLETE              = 0x0200, // CATEGORY:  This node is obsolete and is not available in the editor
    }

    public struct SFlowNodeConfig
    {
        public bool targetsEntity;

        public SInputPortConfig[] pInputPorts;
        public SOutputPortConfig[] pOutputPorts;

        public string sDescription;
        public string sUIClassName;

        public EFlowNodeCategory category;
    }
}