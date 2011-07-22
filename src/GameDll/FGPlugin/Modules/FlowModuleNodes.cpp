/////////////////////////////////////////////////////////////////
// Copyright (C), RenEvo Software & Designs, 2008
// FGPlugin Source File
//
// FlowModuleNodes.cpp
//
// Purpose: Flowgraph Nodes for module usage
//
// History:
//	- 8/03/08 : File created - KAK
/////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "FlowModuleNodes.h"
#include "ModuleManager.h"
#include "Game.h"

#include "FGPSHandler.h"

#define INCLUDE_PARAM(dir,num) dir ## PortConfig_Void("Param" # num, _HELP("" # dir ## " Param " # num))
#define ACTIVATE_PARAM(num) ActivateOutput(&m_actInfo, EOP_Param ## num, params[num-1]);

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////
MODULE_FLOWNODE_STARTNODE::MODULE_FLOWNODE_STARTNODE(SActivationInfo *pActInfo)
{

}

////////////////////////////////////////////////////
MODULE_FLOWNODE_STARTNODE::~MODULE_FLOWNODE_STARTNODE(void)
{

}

////////////////////////////////////////////////////
void MODULE_FLOWNODE_STARTNODE::Serialize(SActivationInfo *pActInfo, TSerialize ser)
{
	if (ser.IsReading())
	{
		m_actInfo = *pActInfo;
	}
}

////////////////////////////////////////////////////
void MODULE_FLOWNODE_STARTNODE::GetConfiguration(SFlowNodeConfig& config)
{
	static const SInputPortConfig inputs[] =
	{
		{0}
	};

	static const SOutputPortConfig outputs[] =
	{
		OutputPortConfig_Void("Start", _HELP("Starting point for module execution")),
		OutputPortConfig<EntityId>("Entity", _HELP("Module entity")),

		INCLUDE_PARAM(Output,1),
		INCLUDE_PARAM(Output,2),
		INCLUDE_PARAM(Output,3),
		INCLUDE_PARAM(Output,4),
		INCLUDE_PARAM(Output,5),
		INCLUDE_PARAM(Output,6),

		{0}
	};

	// Fill in configuration
	config.pInputPorts = inputs;
	config.pOutputPorts = outputs;
	config.sDescription = _HELP("Starting point for a module, outputs data passed in from caller node");
	config.SetCategory(EFLN_APPROVED);
}

////////////////////////////////////////////////////
void MODULE_FLOWNODE_STARTNODE::ProcessEvent(EFlowEvent event, SActivationInfo *pActInfo)
{
	if (eFE_Initialize == event)
	{
		m_actInfo = *pActInfo;
	}
}

////////////////////////////////////////////////////
void MODULE_FLOWNODE_STARTNODE::GetMemoryStatistics(ICrySizer *s)
{
	s->Add(*this);
}

////////////////////////////////////////////////////
IFlowNodePtr MODULE_FLOWNODE_STARTNODE::Clone(SActivationInfo *pActInfo)
{
	return new MODULE_FLOWNODE_STARTNODE(pActInfo);
}

////////////////////////////////////////////////////
void MODULE_FLOWNODE_STARTNODE::OnActivate(EntityId entityId, TModuleParams const& params)
{
	ActivateOutput(&m_actInfo, EOP_Start, true);
	ActivateOutput(&m_actInfo, EOP_Entity, entityId);
	ACTIVATE_PARAM(1);
	ACTIVATE_PARAM(2);
	ACTIVATE_PARAM(3);
	ACTIVATE_PARAM(4);
	ACTIVATE_PARAM(5);
	ACTIVATE_PARAM(6);
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////
MODULE_FLOWNODE_RETURNNODE::MODULE_FLOWNODE_RETURNNODE(SActivationInfo *pActInfo)
{
	m_OwnerId = MODULEID_INVALID;
}

////////////////////////////////////////////////////
MODULE_FLOWNODE_RETURNNODE::~MODULE_FLOWNODE_RETURNNODE(void)
{

}

////////////////////////////////////////////////////
void MODULE_FLOWNODE_RETURNNODE::Serialize(SActivationInfo *pActInfo, TSerialize ser)
{
	if (ser.IsReading())
	{
		m_actInfo = *pActInfo;
		m_OwnerId = MODULEID_INVALID;
	}
}

////////////////////////////////////////////////////
void MODULE_FLOWNODE_RETURNNODE::GetConfiguration(SFlowNodeConfig& config)
{
	static const SInputPortConfig inputs[] =
	{
		InputPortConfig_Void("End", _HELP("Call if module executed successfully")),
		InputPortConfig_Void("Cancel", _HELP("Call if module did not execute successfully")),

		INCLUDE_PARAM(Input,1),
		INCLUDE_PARAM(Input,2),
		INCLUDE_PARAM(Input,3),
		INCLUDE_PARAM(Input,4),
		INCLUDE_PARAM(Input,5),
		INCLUDE_PARAM(Input,6),

		{0}
	};

	static const SOutputPortConfig outputs[] =
	{
		{0}
	};

	// Fill in configuration
	config.pInputPorts = inputs;
	config.pOutputPorts = outputs;
	config.sDescription = _HELP("Returns data back to the caller node");
	config.SetCategory(EFLN_APPROVED);
}

////////////////////////////////////////////////////
void MODULE_FLOWNODE_RETURNNODE::ProcessEvent(EFlowEvent event, SActivationInfo *pActInfo)
{
	if (event == eFE_Initialize)
	{
		m_actInfo = *pActInfo;
		m_OwnerId = MODULEID_INVALID;
	}
	else if (event == eFE_Activate)
	{
		if (IsPortActive(pActInfo, EIP_Succeeded))
		{
			OnActivate(true);
		}
		else if (IsPortActive(pActInfo, EIP_Canceled))
		{
			OnActivate(false);
		}
	}
}

////////////////////////////////////////////////////
void MODULE_FLOWNODE_RETURNNODE::GetMemoryStatistics(ICrySizer *s)
{
	s->Add(*this);
}

////////////////////////////////////////////////////
IFlowNodePtr MODULE_FLOWNODE_RETURNNODE::Clone(SActivationInfo *pActInfo)
{
	return new MODULE_FLOWNODE_RETURNNODE(pActInfo);
}

////////////////////////////////////////////////////
void MODULE_FLOWNODE_RETURNNODE::SetModuleId(TModuleId const& id)
{
	m_OwnerId = id;
}

////////////////////////////////////////////////////
void MODULE_FLOWNODE_RETURNNODE::OnActivate(bool bSuccess)
{
	if (MODULEID_INVALID != m_OwnerId)
	{
		CModuleManager *pModuleManager = g_pGame->GetFGPluginManager()->GetModuleManager();
		assert(pModuleManager);

		// Construct params
		TModuleParams params;
		pModuleManager->GetModuleParamsHolder(params, this, &m_actInfo, EIP_ParamStart);

		// Notify manager
		pModuleManager->OnModuleFinished(m_OwnerId, m_actInfo.pGraph, bSuccess, params);
	}
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////
MODULE_FLOWNODE_CALLERNODE::MODULE_FLOWNODE_CALLERNODE(SActivationInfo *pActInfo)
{

}

////////////////////////////////////////////////////
MODULE_FLOWNODE_CALLERNODE::~MODULE_FLOWNODE_CALLERNODE(void)
{

}

////////////////////////////////////////////////////
void MODULE_FLOWNODE_CALLERNODE::Serialize(SActivationInfo *pActInfo, TSerialize ser)
{
	if (ser.IsReading())
	{
		m_actInfo = *pActInfo;
	}
}

////////////////////////////////////////////////////
void MODULE_FLOWNODE_CALLERNODE::GetConfiguration(SFlowNodeConfig& config)
{
	static const SInputPortConfig inputs[] =
	{
		InputPortConfig_Void("Call", _HELP("Call the module")),
		InputPortConfig<string>("Module", "", _HELP("Name of the module (can exclude \'.xml\')"), 0, 0),
		InputPortConfig<EntityId>("Entity", _HELP("Module entity (applied to Graph Entity or any nodes found without an entity Id assigned")),
		
		INCLUDE_PARAM(Input,1),
		INCLUDE_PARAM(Input,2),
		INCLUDE_PARAM(Input,3),
		INCLUDE_PARAM(Input,4),
		INCLUDE_PARAM(Input,5),
		INCLUDE_PARAM(Input,6),

		{0}
	};

	static const SOutputPortConfig outputs[] =
	{
		OutputPortConfig<bool>("Called", _HELP("Returns TRUE if called, FALSE if error occurred")),
		OutputPortConfig_Void("Done", _HELP("Signaled when module executed successfully")),
		OutputPortConfig_Void("Canceled", _HELP("Signaled when module was canceled")),

		INCLUDE_PARAM(Output,1),
		INCLUDE_PARAM(Output,2),
		INCLUDE_PARAM(Output,3),
		INCLUDE_PARAM(Output,4),
		INCLUDE_PARAM(Output,5),
		INCLUDE_PARAM(Output,6),

		{0}
	};

	// Fill in configuration
	config.pInputPorts = inputs;
	config.pOutputPorts = outputs;
	config.sDescription = _HELP("Call a module");
	config.SetCategory(EFLN_APPROVED);
}

////////////////////////////////////////////////////
void MODULE_FLOWNODE_CALLERNODE::ProcessEvent(EFlowEvent event, SActivationInfo *pActInfo)
{
	if (eFE_Initialize == event)
	{
		m_actInfo = *pActInfo;
	}
	else if (eFE_Activate == event && IsPortActive(pActInfo, EIP_Call))
	{
		CModuleManager *pModuleManager = g_pGame->GetFGPluginManager()->GetModuleManager();
		assert(pModuleManager);

		// Extract and load
		string szModule = GetPortString(pActInfo, EIP_Module);
		EntityId entityId = GetPortEntityId(pActInfo, EIP_Entity);
		TModuleParams params;
		pModuleManager->GetModuleParamsHolder(params, this, pActInfo, EIP_ParamStart);

		const bool bResult = pModuleManager->LoadModule(this, szModule.c_str(), entityId, params);
		ActivateOutput(pActInfo, EOP_Called, bResult);
	}
}

////////////////////////////////////////////////////
void MODULE_FLOWNODE_CALLERNODE::GetMemoryStatistics(ICrySizer *s)
{
	s->Add(*this);
}

////////////////////////////////////////////////////
IFlowNodePtr MODULE_FLOWNODE_CALLERNODE::Clone(SActivationInfo *pActInfo)
{
	return new MODULE_FLOWNODE_CALLERNODE(pActInfo);
}

////////////////////////////////////////////////////
void MODULE_FLOWNODE_CALLERNODE::OnReturn(bool bSuccess, TModuleParams const& params)
{
	if (bSuccess)
	{
		ActivateOutput(&m_actInfo, EOP_Succeeded, true);
	}
	else
	{
		ActivateOutput(&m_actInfo, EOP_Canceled, true);
	}

	ACTIVATE_PARAM(1);
	ACTIVATE_PARAM(2);
	ACTIVATE_PARAM(3);
	ACTIVATE_PARAM(4);
	ACTIVATE_PARAM(5);
	ACTIVATE_PARAM(6);
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

REGISTER_FLOW_NODE(MODULE_FLOWNODE_START, MODULE_FLOWNODE_STARTNODE);
REGISTER_FLOW_NODE(MODULE_FLOWNODE_RETURN, MODULE_FLOWNODE_RETURNNODE);
REGISTER_FLOW_NODE(MODULE_FLOWNODE_CALLER, MODULE_FLOWNODE_CALLERNODE);
