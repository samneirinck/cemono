using System;
using System.IO;
using System.Runtime.CompilerServices;

using System.Reflection;
using System.Collections;
using System.Collections.Generic;

using CryEngine.API;

namespace CryEngine.FlowSystem
{
    [AttributeUsage(AttributeTargets.All)]
    public class NodeCategory : System.Attribute
    {
        public NodeCategory(string category)
        {
            this.category = category;
        }

        public string category;
    }

    public class FlowManager
    {
        FlowManager()
        {
        }

        public void RegisterNodes(string path)
        {
           // Logging.LogAlways("sup");
            /*
            string[] files = Directory.GetFiles(@path, "*.dll", SearchOption.TopDirectoryOnly);

            Assembly plugin;
            object[] customAttributes;
            string category;

            int nodeCounter;

            for (int i = 0; i < files.Length; i++)
            {
                nodeCounter = 0;
                files[i] = files[i].Replace(path, "");

                if (files[i] != "Cemono.Bcl.dll") // Not sure if I can make it stop outputting CryEngine.dll here when building the 
                {
                    plugin = Assembly.LoadFile(path + files[i]);

                    foreach (Type type in plugin.GetTypes())
                    {
                        if (type.IsSubclassOf(typeof(FlowNode)))
                        {
                            category = "NoCategory";

                            customAttributes = type.GetCustomAttributes(typeof(NodeCategory), true);

                            foreach (NodeCategory cat in customAttributes)
                                category = cat.category;

                            nodes.Add(GetNodeId("TestScripted"), new NodeData(type, null)); // Incredibly harcoded hax. TestScripted = last node to get loaded. For some reason, GetNodeId(*name of the last loaded node*) returns the next available node id. Ah well, works for now. TODO: Fix.

                            RegisterNode(category, type.Name);

                            nodeCounter++;
                        }
                    }

                    Logging.LogAlways("[FGPS] Registered {0} nodes in {1}", nodeCounter, files[i]);
                }
            }*/
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern protected static void _RegisterNode(string category, string nodeName);

        public void RegisterNode(string category, string nodeName)
        {
            _RegisterNode(category, nodeName);
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern protected static UInt16 _GetNodeId(string nodeName);

        public UInt16 GetNodeId(string nodeName)
        {
            return _GetNodeId(nodeName);
        }

        void NodeProcessEvent(FlowNode.EFlowEvent flowEvent, UInt16 nodeId)
        {
            Logging.LogAlways("Process event on node of type {0}", nodeId);

            object[] args = new object[2];
            args[0] = flowEvent;
            args[1] = nodeId;

            InvokeNodeFunc(nodeId, "ProcessEvent", args);
        }

        SFlowNodeConfig GetConfiguration(UInt16 nodeId)
        {
            return (SFlowNodeConfig)InvokeNodeFunc(nodeId, "GetConfiguration");
        }

        object InvokeNodeFunc(UInt16 nodeId, string func, object[] args = null)
        {
            if (nodes[nodeId].obj == null)
            {
                NodeData data = nodes[nodeId];

                data.obj = Activator.CreateInstance(nodes[nodeId].type);

                nodes[nodeId] = data;
            }

            return nodes[nodeId].type.InvokeMember(func,
                BindingFlags.DeclaredOnly |
                BindingFlags.Public | BindingFlags.NonPublic |
                BindingFlags.Instance | BindingFlags.InvokeMethod, null, nodes[nodeId].obj, args);
        }

        Dictionary<UInt16, NodeData> nodes = new Dictionary<UInt16, NodeData>();
    }

    public struct NodeData
    {
        public NodeData(Type nodeType, object _obj)
        {
            type = nodeType;
            obj = _obj;
        }

        public Type type;
        public object obj;
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

    public enum EOutputPortType
    {
        Void
    }

    public struct SOutputPortConfig
    {
        public SOutputPortConfig(string sName, EOutputPortType outType, string sDescription = null, string sHumanName = null)
        {
            name = sName;
            description = sDescription;
            humanName = sHumanName;

            outputType = outType;
        }

        // name of this port
        public string name;
        // Human readable name of this port (default: same as name)
        public string humanName;
        // Human readable description of this port (help)
        public string description;

        public EOutputPortType outputType;
    }

    public enum EFlowNodeFlags
    {
        EFLN_TARGET_ENTITY = 0x0001, // CORE FLAG: This node targets an entity, entity id must be provided.
        EFLN_HIDE_UI = 0x0002, // CORE FLAG: This node cannot be selected by user for placement in flow graph UI.
        EFLN_CORE_MASK = 0x000F, // CORE_MASK

        EFLN_APPROVED = 0x0010, // CATEGORY:  This node is approved for designers
        EFLN_ADVANCED = 0x0020, // CATEGORY:  This node is slightly advanced and approved
        EFLN_DEBUG = 0x0040, // CATEGORY:  This node is for debug purpose only
        EFLN_WIP = 0x0080, // CATEGORY:  This node is work-in-progress and shouldn't be used by designers
        EFLN_LEGACY = 0x0100, // CATEGORY:  This node is legacy and will VERY soon vanish
        EFLN_OBSOLETE = 0x0200, // CATEGORY:  This node is obsolete and is not available in the editor
        //EFLN_NOCATEGORY            = 0x0800, // CATEGORY:  This node has no category yet! Default!
        EFLN_CATEGORY_MASK = 0x0FF0, // CATEGORY_MASK

        EFLN_USAGE_MASK = 0xF000, // USAGE_MASK
    }

    public struct SFlowNodeConfig
    {
        public SInputPortConfig[] inputs
        {
            get { return inputPorts; }
            set { inputPorts = value; inputSize = inputPorts.Length; }
        }

        public SOutputPortConfig[] outputs
        {
            get { return outputPorts; }
            set { outputPorts = value; outputSize = outputPorts.Length; }
        }

        public string description
        {
            get { return sDescription; }
            set { sDescription = value; }
        }

        public EFlowNodeFlags category
        {
            get { return eCategory; }
            set { eCategory = value; }
        }

        private SInputPortConfig[] inputPorts;
        private SOutputPortConfig[] outputPorts;

        private int inputSize;
        private int outputSize;

        private string sDescription;
        private string sUIClassName;

        private EFlowNodeFlags eCategory;
    }
}