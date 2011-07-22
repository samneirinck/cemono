/////////////////////////////////////////////////////////////////
// Copyright (C), RenEvo Software & Designs, 2008
// FGPlugin Source File
//
// FlowModuleNodes.h
//
// Purpose: Flowgraph Nodes for module usage
//
// History:
//	- 8/03/08 : File created - KAK
/////////////////////////////////////////////////////////////////

#ifndef _FLOWMODULENODES_H_
#define _FLOWMODULENODES_H_

#include "ModuleCommon.h"
#include "G2FlowBaseNode.h"

#define DEFINE_PARAM(dir,num) E ## dir ## P_Param ## num

////////////////////////////////////////////////////
class MODULE_FLOWNODE_STARTNODE : public CFlowBaseNode
{
	enum EOutputPorts
	{
		EOP_Start,
		EOP_Entity,

		EOP_ParamStart,
		DEFINE_PARAM(O,1) = EOP_ParamStart,
		DEFINE_PARAM(O,2),
		DEFINE_PARAM(O,3),
		DEFINE_PARAM(O,4),
		DEFINE_PARAM(O,5),
		DEFINE_PARAM(O,6),
	};

	SActivationInfo m_actInfo;

public:
	////////////////////////////////////////////////////
	MODULE_FLOWNODE_STARTNODE(SActivationInfo *pActInfo);
	virtual ~MODULE_FLOWNODE_STARTNODE(void);

	////////////////////////////////////////////////////
	virtual void Serialize(SActivationInfo *pActInfo, TSerialize ser);
	virtual void GetConfiguration(SFlowNodeConfig& config);
	virtual void ProcessEvent(EFlowEvent event, SActivationInfo *pActInfo);
	virtual void GetMemoryStatistics(ICrySizer *s);
	virtual IFlowNodePtr Clone(SActivationInfo *pActInfo);

	////////////////////////////////////////////////////
	void OnActivate(EntityId entityId, TModuleParams const& params);
};

////////////////////////////////////////////////////
class MODULE_FLOWNODE_RETURNNODE : public CFlowBaseNode
{
	enum EInputPorts
	{
		EIP_Succeeded,
		EIP_Canceled,

		EIP_ParamStart,
		DEFINE_PARAM(I,1) = EIP_ParamStart,
		DEFINE_PARAM(I,2),
		DEFINE_PARAM(I,3),
		DEFINE_PARAM(I,4),
		DEFINE_PARAM(I,5),
		DEFINE_PARAM(I,6),
	};

	TModuleId m_OwnerId;
	SActivationInfo m_actInfo;

public:
	////////////////////////////////////////////////////
	MODULE_FLOWNODE_RETURNNODE(SActivationInfo *pActInfo);
	virtual ~MODULE_FLOWNODE_RETURNNODE(void);

	////////////////////////////////////////////////////
	virtual void Serialize(SActivationInfo *pActInfo, TSerialize ser);
	virtual void GetConfiguration(SFlowNodeConfig& config);
	virtual void ProcessEvent(EFlowEvent event, SActivationInfo *pActInfo);
	virtual void GetMemoryStatistics(ICrySizer *s);
	virtual IFlowNodePtr Clone(SActivationInfo *pActInfo);

	////////////////////////////////////////////////////
	void SetModuleId(TModuleId const& id);
	void OnActivate(bool bSuccess);
};

////////////////////////////////////////////////////
class MODULE_FLOWNODE_CALLERNODE : public CFlowBaseNode
{
	enum EInputPorts
	{
		EIP_Call,
		EIP_Module,
		EIP_Entity,

		EIP_ParamStart,
		DEFINE_PARAM(I,1) = EIP_ParamStart,
		DEFINE_PARAM(I,2),
		DEFINE_PARAM(I,3),
		DEFINE_PARAM(I,4),
		DEFINE_PARAM(I,5),
		DEFINE_PARAM(I,6),
	};

	enum EOutputPorts
	{
		EOP_Called,
		EOP_Succeeded,
		EOP_Canceled,

		EOP_ParamStart,
		DEFINE_PARAM(O,1) = EOP_ParamStart,
		DEFINE_PARAM(O,2),
		DEFINE_PARAM(O,3),
		DEFINE_PARAM(O,4),
		DEFINE_PARAM(O,5),
		DEFINE_PARAM(O,6),
	};

	SActivationInfo m_actInfo;

public:
	////////////////////////////////////////////////////
	MODULE_FLOWNODE_CALLERNODE(SActivationInfo *pActInfo);
	virtual ~MODULE_FLOWNODE_CALLERNODE(void);

	////////////////////////////////////////////////////
	virtual void Serialize(SActivationInfo *pActInfo, TSerialize ser);
	virtual void GetConfiguration(SFlowNodeConfig& config);
	virtual void ProcessEvent(EFlowEvent event, SActivationInfo *pActInfo);
	virtual void GetMemoryStatistics(ICrySizer *s);
	virtual IFlowNodePtr Clone(SActivationInfo *pActInfo);

	////////////////////////////////////////////////////
	void OnReturn(bool bSuccess, TModuleParams const& params);
};

#endif //_FLOWMODULENODES_H_
