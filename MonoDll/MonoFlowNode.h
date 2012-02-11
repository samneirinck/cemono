/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// Flow node class used to forward events to Mono scripts.
//////////////////////////////////////////////////////////////////////////
// 23/12/2011 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __MONO_FLOWBASENODE_H__
#define __MONO_FLOWBASENODE_H__

#include "MonoCommon.h"
#include <IMonoObject.h>
#include <IMonoArray.h>

#include "FlowManager.h"

#include <IMonoScriptSystem.h>
#include <IFlowSystem.h>

struct IMonoArray;
struct IMonoScript;

class CMonoFlowNode : public IFlowNode, public IFlowGraphHook
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

	CMonoFlowNode(SActivationInfo *pActInfo, bool isEntityClass);
	~CMonoFlowNode();
	//////////////////////////////////////////////////////////////////////////
	// IFlowNode
	virtual void AddRef() { ++m_refs; };
	virtual void Release() { if (0 >= --m_refs)	delete this; };

	virtual IFlowNodePtr Clone( SActivationInfo *pActInfo ) { return new CMonoFlowNode(pActInfo, m_bEntityNode); }
	virtual bool SerializeXML( SActivationInfo *, const XmlNodeRef&, bool ) { return true; }
	virtual void Serialize(SActivationInfo *, TSerialize ser) {}
	virtual void ProcessEvent( EFlowEvent event, SActivationInfo *pActInfo );

	virtual void GetMemoryUsage(ICrySizer * s) const { s->Add(*this); }
	//////////////////////////////////////////////////////////////////////////

	// IFlowGraphHook
	virtual IFlowNodePtr CreateNode( IFlowNode::SActivationInfo*, TFlowNodeTypeId typeId ) { return NULL; }
	virtual bool CreatedNode( TFlowNodeId id, const char * name, TFlowNodeTypeId typeId, IFlowNodePtr pNode );
	virtual void CancelCreatedNode( TFlowNodeId id, const char * name, TFlowNodeTypeId typeId, IFlowNodePtr pNode ) {}
	// ~IFlowGraphHook

	virtual void GetConfiguration( SFlowNodeConfig& );
	// ~IFlowNode

	bool IsPortActive(int nPort) const
	{
		return m_pActInfo->pInputPorts[nPort].IsUserFlagSet();
	}
	bool IsBoolPortActive(int nPort) const
	{
		if (IsPortActive(nPort) && GetPortBool(nPort))
			return true;
		else
			return false;
	}
	EFlowDataTypes GetPortType(int nPort) const
	{
		return (EFlowDataTypes)m_pActInfo->pInputPorts[nPort].GetType();
	}

	const TFlowInputData& GetPortAny(int nPort) const
	{
		return m_pActInfo->pInputPorts[nPort];
	}

	bool GetPortBool(int nPort) const
	{
		bool* p_x = (m_pActInfo->pInputPorts[nPort].GetPtr<bool>());
		if (p_x != 0) return *p_x;
		SFlowNodeConfig config;
		const_cast<CMonoFlowNode *> (this)->GetConfiguration(config);
		GameWarning("CFlowBaseNode::GetPortBool: Node=%p Port=%d '%s' Tag=%d -> Not a bool tag!", this, nPort,
			config.pInputPorts[nPort].name,
			m_pActInfo->pInputPorts[nPort].GetTag());
		return false;
	}
	int GetPortInt(int nPort) const
	{
		int x = *(m_pActInfo->pInputPorts[nPort].GetPtr<int>());
		return x;
	}
	EntityId GetPortEntityId(int nPort)
	{
		EntityId x = *(m_pActInfo->pInputPorts[nPort].GetPtr<EntityId>());
		return x;
	}
	float GetPortFloat(int nPort) const
	{
		float x = *(m_pActInfo->pInputPorts[nPort].GetPtr<float>());
		return x;
	}
	Vec3 GetPortVec3(int nPort) const
	{
		Vec3 x = *(m_pActInfo->pInputPorts[nPort].GetPtr<Vec3>());
		return x;
	}
	EntityId GetPortEntityId(int nPort) const
	{
		EntityId x = *(m_pActInfo->pInputPorts[nPort].GetPtr<EntityId>());
		return x;
	}
	const string& GetPortString(int nPort) const
	{
		const string* p_x = (m_pActInfo->pInputPorts[nPort].GetPtr<string>());
		if (p_x != 0) return *p_x;
		const static string empty ("");
		SFlowNodeConfig config;
		const_cast<CMonoFlowNode*> (this)->GetConfiguration(config);
		GameWarning("CMonoFlowNode::GetPortString: Node=%p Port=%d '%s' Tag=%d -> Not a string tag!", this, nPort,
			config.pInputPorts[nPort].name,
			m_pActInfo->pInputPorts[nPort].GetTag());
		return empty;
	}

	//////////////////////////////////////////////////////////////////////////
	// Sends data to output port.
	//////////////////////////////////////////////////////////////////////////
	template <class T>
		void ActivateOutput(int nPort, const T &value )
	{
		SFlowAddress addr( m_pActInfo->myID, nPort, true );
		m_pActInfo->pGraph->ActivatePort( addr, value );
	}
	//////////////////////////////////////////////////////////////////////////
	bool IsOutputConnected(int nPort) const
	{
		SFlowAddress addr( m_pActInfo->myID, nPort, true );
		return m_pActInfo->pGraph->IsOutputConnected( addr );
	}
	//////////////////////////////////////////////////////////////////////////

	TFlowNodeId GetId() const { return m_pActInfo->myID; }
	int GetScriptId() const { return m_scriptId; }

	void SetRegularlyUpdated(bool update) { m_pActInfo->pGraph->SetRegularlyUpdated(m_pActInfo->myID, update); }

private:

	// Returns true if successful
	virtual bool InstantiateScript(const char *nodeName);

	SActivationInfo *m_pActInfo;
	IFlowGraph *m_pHookedGraph;

	int m_scriptId;

	bool m_bInitialized;
	bool m_bEntityNode;

	int m_refs;
};

template <class T>
ILINE SInputPortConfig MonoInputPortConfig(mono::string name, mono::object defaultVal, mono::string humanName, mono::string desc, mono::string UIConfig)
{
	IMonoObject *pObject = *defaultVal;
	SInputPortConfig config = InputPortConfig<T>(ToCryString(name), pObject->Unbox<T>(), ToCryString(humanName), _HELP(ToCryString(desc)), ToCryString(UIConfig));

	SAFE_RELEASE(pObject);
	return config;
}

struct SMonoInputPortConfig
{
	SInputPortConfig Convert()
	{
		switch(type)
		{
		case eFDT_Void:
			{
				return InputPortConfig_Void(ToCryString(name), _HELP(ToCryString(description)), ToCryString(humanName), ToCryString(uiConfig));
			}
			break;
		case eFDT_Int:
			{
				return MonoInputPortConfig<int>(name, defaultValue, description, humanName, uiConfig);
			}
			break;
		case eFDT_Float:
			{
				return MonoInputPortConfig<float>(name, defaultValue, description, humanName, uiConfig);
			}
			break;
		case eFDT_EntityId:
			{
				return MonoInputPortConfig<EntityId>(name, defaultValue, description, humanName, uiConfig);
			}
			break;
		case eFDT_Vec3:
			{
				return MonoInputPortConfig<Vec3>(name, defaultValue, description, humanName, uiConfig);
			}
			break;
		case eFDT_String:
			{
				return InputPortConfig<string>(ToCryString(name), ToCryString((mono::string)defaultValue), ToCryString(humanName), _HELP(ToCryString(description)), ToCryString(uiConfig));
			}
			break;
		case eFDT_Bool:
			{
				return MonoInputPortConfig<bool>(name, defaultValue, description, humanName, uiConfig);
			}
			break;
		}

		return *(SInputPortConfig *)0;
	}
	mono::string name;
	mono::string humanName;
	mono::string description;

	EFlowDataTypes type;

	mono::string uiConfig;

	mono::object defaultValue;
};

struct SMonoOutputPortConfig
{
	SOutputPortConfig Convert()
	{
		const char *sName = ToCryString(name);
		const char *sHumanName = ToCryString(humanName);
		const char *sDesc = ToCryString(description);

		switch(type)
		{
		case eFDT_Void:
			{
				return OutputPortConfig_Void(sName, _HELP(sDesc), sHumanName);
			}
		case eFDT_Int:
			{
				return OutputPortConfig<int>(sName, _HELP(sDesc), sHumanName);
			}
			break;
		case eFDT_Float:
			{
				return OutputPortConfig<float>(sName, _HELP(sDesc), sHumanName);
			}
			break;
		case eFDT_EntityId:
			{
				return OutputPortConfig<EntityId>(sName, _HELP(sDesc), sHumanName);
			}
			break;
		case eFDT_Vec3:
			{
				return OutputPortConfig<Vec3>(sName, _HELP(sDesc), sHumanName);
			}
			break;
		case eFDT_String:
			{
				return OutputPortConfig<string>(sName, _HELP(sDesc), sHumanName);
			}
			break;
		case eFDT_Bool:
			{
				return OutputPortConfig<bool>(sName, _HELP(sDesc), sHumanName);
			}
			break;
		}

		return *(SOutputPortConfig *)0;
	}

	mono::string name;
	mono::string humanName;
	mono::string description;

	EFlowDataTypes type;
};


struct SMonoNodeConfig
{
	EFlowNodeFlags flags;
	EFlowNodeFlags category;

	mono::string description;
};

struct SMonoNodePortConfig
{
	mono::array inputs;
	mono::array outputs;
};

#endif // __MONO_FLOWBASENODE_H__