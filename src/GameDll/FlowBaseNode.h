#ifndef __FLOWBASENODE_H__
#define __FLOWBASENODE_H__

#include <IFlowSystem.h>

class CFlowBaseNode : public IFlowNode
{
public:
	CFlowBaseNode() { m_refs = 0; };
	virtual ~CFlowBaseNode() {}

	//////////////////////////////////////////////////////////////////////////
	// IFlowNode
	virtual void AddRef() { ++m_refs; };
	virtual void Release() { if (0 >= --m_refs)	delete this; };

	virtual IFlowNodePtr Clone( SActivationInfo *pActInfo ) { return this; }
	virtual bool SerializeXML( SActivationInfo *, const XmlNodeRef&, bool ) { return true; }
	virtual void Serialize(SActivationInfo *, TSerialize ser) {}
	virtual void ProcessEvent( EFlowEvent event, SActivationInfo *pActInfo ) {};
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Common functions to use in derived classes.
	//////////////////////////////////////////////////////////////////////////
	bool IsPortActive( SActivationInfo *pActInfo,int nPort ) const
	{
		return pActInfo->pInputPorts[nPort].IsUserFlagSet();
	}
	bool IsBoolPortActive( SActivationInfo *pActInfo,int nPort ) const
	{
		if (IsPortActive(pActInfo,nPort) && GetPortBool(pActInfo,nPort))
			return true;
		else
			return false;
	}
	EFlowDataTypes GetPortType( SActivationInfo *pActInfo,int nPort ) const
	{
		return (EFlowDataTypes)pActInfo->pInputPorts[nPort].GetType();
	}

	const TFlowInputData& GetPortAny( SActivationInfo *pActInfo,int nPort ) const
	{
		return pActInfo->pInputPorts[nPort];
	}

	bool GetPortBool( SActivationInfo *pActInfo,int nPort ) const
	{
		bool* p_x = (pActInfo->pInputPorts[nPort].GetPtr<bool>());
		if (p_x != 0) return *p_x;
		SFlowNodeConfig config;
		const_cast<CFlowBaseNode*> (this)->GetConfiguration(config);
		GameWarning("CFlowBaseNode::GetPortBool: Node=%p Port=%d '%s' Tag=%d -> Not a bool tag!", this, nPort,
			config.pInputPorts[nPort].name,
			pActInfo->pInputPorts[nPort].GetTag());
		return false;
	}
	int GetPortInt( SActivationInfo *pActInfo,int nPort ) const
	{
		int x = *(pActInfo->pInputPorts[nPort].GetPtr<int>());
		return x;
	}
	EntityId GetPortEntityId( SActivationInfo *pActInfo,int nPort )
	{
		EntityId x = *(pActInfo->pInputPorts[nPort].GetPtr<EntityId>());
		return x;
	}
	float GetPortFloat( SActivationInfo *pActInfo,int nPort ) const
	{
		float x = *(pActInfo->pInputPorts[nPort].GetPtr<float>());
		return x;
	}
	Vec3 GetPortVec3( SActivationInfo *pActInfo,int nPort ) const
	{
		Vec3 x = *(pActInfo->pInputPorts[nPort].GetPtr<Vec3>());
		return x;
	}
	EntityId GetPortEntityId( SActivationInfo *pActInfo,int nPort ) const
	{
		EntityId x = *(pActInfo->pInputPorts[nPort].GetPtr<EntityId>());
		return x;
	}
	const string& GetPortString( SActivationInfo *pActInfo,int nPort ) const
	{
		const string* p_x = (pActInfo->pInputPorts[nPort].GetPtr<string>());
		if (p_x != 0) return *p_x;
		const static string empty ("");
		SFlowNodeConfig config;
		const_cast<CFlowBaseNode*> (this)->GetConfiguration(config);
		GameWarning("CFlowBaseNode::GetPortString: Node=%p Port=%d '%s' Tag=%d -> Not a string tag!", this, nPort,
			config.pInputPorts[nPort].name,
			pActInfo->pInputPorts[nPort].GetTag());
		return empty;
	}

	//////////////////////////////////////////////////////////////////////////
	// Sends data to output port.
	//////////////////////////////////////////////////////////////////////////
	template <class T>
		void ActivateOutput( SActivationInfo *pActInfo,int nPort, const T &value )
	{
		SFlowAddress addr( pActInfo->myID, nPort, true );
		pActInfo->pGraph->ActivatePort( addr, value );
	}
	//////////////////////////////////////////////////////////////////////////
	bool IsOutputConnected( SActivationInfo *pActInfo,int nPort ) const
	{
		SFlowAddress addr( pActInfo->myID, nPort, true );
		return pActInfo->pGraph->IsOutputConnected( addr );
	}
	//////////////////////////////////////////////////////////////////////////

private:
	int m_refs;
};

#endif // __FLOWBASENODE_H__