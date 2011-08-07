using System;
using System.IO;
using System.Runtime.CompilerServices;

using System.Reflection;
using System.Collections.Generic;

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

        private void RegisterNodes(string path)
        {
            string[] files = Directory.GetFiles(@path, "*.dll", SearchOption.TopDirectoryOnly);

            Assembly plugin;
            object[] customAttributes;
            string category = "Default";

            for (int i = 0; i < files.Length; i++)
            {
                files[i] = files[i].Replace(path, "");

                if (files[i] == "CryEngine.dll") // Skip this, if it was placed in here for some reason.
                    files[i] = null;

                if (files[i] != null)
                {
                    CryConsole.LogAlways("Loading nodes in file {0}", files[i]);

                    plugin = Assembly.LoadFile(path + files[i]);

                    foreach (Type type in plugin.GetTypes())
                    {
                        if (type.IsSubclassOf(typeof(IFlowNode)))
                        {
                            customAttributes = type.GetCustomAttributes(typeof(NodeCategory), true);

                            foreach (NodeCategory cat in customAttributes)
                                category = cat.category;

                            nodeTypes.Add(type.Name, type);

                            RegisterNode(category, type.Name);
                        }
                    }
                }
            }
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern protected static void _RegisterNode(string category, string nodeName);

        public void RegisterNode(string category, string nodeName)
        {
            _RegisterNode(category, nodeName);
        }

        void NodeProcessEvent(IFlowNode.EFlowEvent flowEvent, UInt16 nodeId)
        {
            CryConsole.LogAlways("Process event on node of type {0}", nodeId);

            InvokeNodeFunc(nodeId, "ProcessEvent");
        }

        SFlowNodeConfig GetConfiguration(UInt16 nodeId)
        {
            return (SFlowNodeConfig)InvokeNodeFunc(nodeId, "GetConfiguration");
        }

        object InvokeNodeFunc(UInt16 nodeId, string func)
        {
            CryConsole.LogAlways("Mono.1");

            if (nodes.ContainsKey(nodeId))
            {
                CryConsole.LogAlways("Mono.2");

                return nodes[nodeId].type.InvokeMember(func,
                BindingFlags.DeclaredOnly |
                BindingFlags.Public | BindingFlags.NonPublic |
                BindingFlags.Instance | BindingFlags.InvokeMethod, null, nodes[nodeId].obj, null);
            }
            else
            {
                CryConsole.LogAlways("Mono.3");

                Type type = nodeTypes["SampleNode"];

                CryConsole.LogAlways("Mono.4");
                
                // Construct
                object typeInstance = Activator.CreateInstance(type); // crash

                CryConsole.LogAlways("Mono.5");

                nodes.Add(nodeId, new NodeData(type, typeInstance));

                CryConsole.LogAlways("Mono.6");

                return InvokeNodeFunc(nodeId, func);
            }
        }

        Dictionary<UInt16, NodeData> nodes = new Dictionary<UInt16, NodeData>();
        Dictionary<string, Type> nodeTypes = new Dictionary<string, Type>();
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
            get
            {
                return inputPorts;
            }
            set
            {
                inputPorts = value;

                inputSize = inputPorts.Length;
            }
        }

        public SOutputPortConfig[] outputs
        {
            get
            {
                return outputPorts;
            }
            set
            {
                outputPorts = value;

                outputSize = outputPorts.Length;
            }
        }

        public string description
        {
            get
            {
                return sDescription;
            }
            set
            {
                sDescription = value;
            }
        }

        public EFlowNodeFlags category
        {
            get
            {
                return eCategory;
            }
            set
            {
                eCategory = value;
            }
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