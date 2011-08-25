using System;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using System.Diagnostics;
using System.Runtime.CompilerServices;

namespace CryEngine.FlowSystem
{
    public class FlowManager
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern private static void _RegisterNode(string category, string nodeName);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern private static UInt16 _GetNodeId(string nodeName);

        public static void RegisterNode(string category, string nodeName)
        {
            _RegisterNode(category, nodeName);
        }
        public UInt16 GetNodeId(string nodeName)
        {
            return _GetNodeId(nodeName);
        }

        public void RegisterNodes(string path)
        {
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

                            customAttributes = type.GetCustomAttributes(typeof(NodeCategoryAttribute), true);

                            foreach (NodeCategoryAttribute cat in customAttributes)
                                category = cat.Category;

                            nodes.Add(GetNodeId("TestScripted"), new NodeData(type, null)); // Incredibly harcoded hax. TestScripted = last node to get loaded. For some reason, GetNodeId(*name of the last loaded node*) returns the next available node id. Ah well, works for now. TODO: Fix.

                            RegisterNode(category, type.Name);

                            nodeCounter++;
                        }
                    }

                    Trace.TraceInformation("[FGPS] Registered {0} nodes in {1}", nodeCounter, files[i]);
                }
            }
        }


        void NodeProcessEvent(FlowNode.EFlowEvent flowEvent, UInt16 nodeId)
        {
            Trace.TraceInformation("Process event on node of type {0}", nodeId);

            object[] args = new object[2];
            args[0] = flowEvent;
            args[1] = nodeId;

            InvokeNodeFunc(nodeId, "ProcessEvent", args);
        }

        FlowNodeConfig GetConfiguration(UInt16 nodeId)
        {
            return (FlowNodeConfig)InvokeNodeFunc(nodeId, "GetConfiguration");
        }

        object InvokeNodeFunc(UInt16 nodeId, string func, object[] args = null)
        {
            if (nodes[nodeId].Object == null)
            {
                NodeData data = nodes[nodeId];

                data.Object = Activator.CreateInstance(nodes[nodeId].Type);

                nodes[nodeId] = data;
            }

            return nodes[nodeId].Type.InvokeMember(func,
                BindingFlags.DeclaredOnly |
                BindingFlags.Public | BindingFlags.NonPublic |
                BindingFlags.Instance | BindingFlags.InvokeMethod, null, nodes[nodeId].Object, args);
        }

        Dictionary<UInt16, NodeData> nodes = new Dictionary<UInt16, NodeData>();
    }
}
