using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using CryEngine;
using CryEngine.FlowSystem;

namespace FGPlugin
{
    class PluginMain
    {
        public SPluginRegister RegisterWithPluginSystem()
        {
            SPluginRegister pluginRegister = new SPluginRegister(true);

            pluginRegister.nodesFirst = CG2AutoRegFlowNodeBase.m_pFirst;
            pluginRegister.nodesLast = CG2AutoRegFlowNodeBase.m_pLast;

            return pluginRegister;
        }

        static void RegisterNode<T> (string className, IFlowNode nodeClass)
        {
            CG2AutoRegFlowNodeBase nodeFirst = new CG2AutoRegFlowNode<T>(className);
            CG2AutoRegFlowNodeBase.m_pFirst = nodeFirst;
            /*
            CG2AutoRegFlowNodeBase node = CG2AutoRegFlowNodeBase.m_pFirst;
	        if (node!=null)
	        {
		        while (node!=null)
		        {
			        node = node.m_pNext;
		        }

		        // Add to flowgraph registration list
                CG2AutoRegFlowNodeBase.m_pLast.m_pNext = nodeFirst;
		        //CG2AutoRegFlowNodeBase.m_pLast = registerResults.nodesLast;
	        }*/
        }
    }
}
