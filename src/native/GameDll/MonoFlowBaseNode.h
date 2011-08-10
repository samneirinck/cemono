#ifndef __MONO_FLOWBASENODE_H__
#define __MONO_FLOWBASENODE_H__

#include <IFlowSystem.h>

#include "FlowBaseNode.h"
#include "Mono.h"

class CMonoFlowNode : public CFlowBaseNode, public IFlowNodeFactory
{
public:
	enum EInputPorts
	{
		EIP_Start,
		EIP_Cancel
	};

	enum EOutputPorts
	{
		EOP_Started,
		EOP_Cancelled
	};

	CMonoFlowNode() {}
	CMonoFlowNode(SActivationInfo *pActInfo);
	~CMonoFlowNode();

	virtual void AddRef() {}
	virtual void Release() {}
	IFlowNodePtr Create( IFlowNode::SActivationInfo *pActInfo ) { return new CMonoFlowNode(pActInfo); }
	virtual IFlowNodePtr Clone( SActivationInfo *pActInfo ) { return new CMonoFlowNode(pActInfo); }

	virtual void GetConfiguration( SFlowNodeConfig& );
	virtual bool SerializeXML( SActivationInfo *, const XmlNodeRef& root, bool reading ) { return true; }
	virtual void Serialize( SActivationInfo *, TSerialize ser ) {}
	virtual void ProcessEvent( EFlowEvent event, SActivationInfo * );

	//const char *GetNodeName(SActivationInfo *);

	virtual void GetMemoryStatistics(ICrySizer * s) 
	{ 
		SIZER_SUBCOMPONENT_NAME(s, "CMonoFlowNode");
		s->Add(*this); 
	}

	SActivationInfo *m_pActInfo;
};

enum EMonoInputPortType
{
	EMInputType_Void,
	EMInputType_String,
	EMInputType_Int
};

struct SMonoInputPortConfig
{
	MonoString *name;
	MonoString *humanName;
	MonoString *description;
	MonoString *sUIConfig;

	EMonoInputPortType inputType;
};

enum EMonoOutputPortType
{
	EMOutputType_Void
};

struct SMonoOutputPortConfig
{
	MonoString *name;
	MonoString *humanName;
	MonoString *description;

	EMonoOutputPortType outputType;
};

struct SMonoFlowNodeConfig
{
	MonoArray *pInputPorts; // SMonoInputPortConfig
	MonoArray *pOutputPorts; // SMonoOutputPortConfig

	int inputSize;
	int outputSize;

	MonoString *sDescription;
	MonoString *sUIClassName;

	EFlowNodeFlags flags; 
};

ILINE SInputPortConfig MonoInputPortConfig(EMonoInputPortType inputType, const char * name, const char *description=NULL, const char *humanName=NULL, const char *sUIConfig=NULL)
{
	SInputPortConfig config;

	switch(inputType)
	{
	case EMInputType_Void:
		config = InputPortConfig_Void(name, description, humanName, sUIConfig);
		break;
	case EMInputType_String:
		config = InputPortConfig<string>(name, description, humanName, sUIConfig);
		break;
	case EMInputType_Int:
		config = InputPortConfig<int>(name, description, humanName, sUIConfig);
		break;
	}

	return config;
}

ILINE SOutputPortConfig MonoOutputPortConfig(EMonoOutputPortType outputType, const char * name, const char *description=NULL, const char *humanName=NULL, const char *sUIConfig=NULL)
{
	SOutputPortConfig config;

	switch(outputType)
	{
	case EMInputType_Void:
		config = OutputPortConfig_Void(name, description, humanName);
		break;
	}

	return config;
}

#endif // __MONO_FLOWBASENODE_H__