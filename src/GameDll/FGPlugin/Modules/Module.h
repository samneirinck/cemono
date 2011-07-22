/////////////////////////////////////////////////////////////////
// Copyright (C), RenEvo Software & Designs, 2008
// FGPlugin Source File
//
// Module.h
//
// Purpose: Module container
//
// History:
//	- 8/03/08 : File created - KAK
/////////////////////////////////////////////////////////////////

#ifndef _MODULE_H_
#define _MODULE_H_

#include "ModuleCommon.h"

class CModule
{
	TModuleId m_Id;

	// Root graph all others are cloned from
	IFlowGraphPtr m_pRootGraph;

	// Instance def
	struct SInstance
	{
		IFlowGraphPtr pGraph;
		MODULE_FLOWNODE_CALLERNODE *pCaller;
		bool bUsed;

		SInstance() : pGraph(NULL), pCaller(NULL), bUsed(false) {}
	};

	// Instance graphs
	typedef std::list<SInstance> TInstanceList;
	TInstanceList m_instances;

public:
	////////////////////////////////////////////////////
	CModule();
	virtual ~CModule();

	////////////////////////////////////////////////////
	TModuleId const& GetId() const;

	////////////////////////////////////////////////////
	// Init
	//
	// Purpose: Initialize base graph
	////////////////////////////////////////////////////
	bool Init(char const* module, TModuleId const& id);

	////////////////////////////////////////////////////
	// Destroy
	//
	// Purpose: Clean up
	////////////////////////////////////////////////////
	void Destroy();

	////////////////////////////////////////////////////
	// CreateInstance
	//
	// Purpose: Load an instance of a graph with given
	//	content
	////////////////////////////////////////////////////
	bool CreateInstance(MODULE_FLOWNODE_CALLERNODE* pCaller, EntityId const& entityId, TModuleParams const& params);

	////////////////////////////////////////////////////
	// DestroyInstance
	//
	// Purpose: Destroys an instance and cleans up
	////////////////////////////////////////////////////
	bool DestroyInstance(IFlowGraph *pInstance, bool bSuccess, TModuleParams const& params);

private:
	////////////////////////////////////////////////////
	// LoadModuleFile
	//
	// Purpose: Loads a module file
	////////////////////////////////////////////////////
	bool LoadModuleFile(char const* module, XmlNodeRef &ref) const;

	////////////////////////////////////////////////////
	// ResolveAddress
	//
	// Purpose: Resolves a port address
	////////////////////////////////////////////////////
	bool ResolveAddress(SFlowAddress &addr, IFlowGraph *pGraph, TFlowNodeId nodeId, char const* portName, bool bIsOutput = false) const;

	////////////////////////////////////////////////////
	// Warning
	//
	// Purpose: Output formatted warning message
	////////////////////////////////////////////////////
	void Warning(char const* file, char const* msg, ...) const;

private:
	////////////////////////////////////////////////////
	// ActivateGraph
	//
	// Purpose: Activates the graph by finding the Start
	//	nodes and calling them
	////////////////////////////////////////////////////
	void ActivateGraph(IFlowGraph *pGraph, EntityId const& entityId, TModuleParams const& params) const;

	////////////////////////////////////////////////////
	// DeactivateGraph
	//
	// Purpose: Deactivates the graph by notifying the
	//	original caller
	////////////////////////////////////////////////////
	void DeactivateGraph(SInstance &instance, bool bSuccess, TModuleParams const& params) const;
};

TYPEDEF_AUTOPTR(CModule);
typedef CModule_AutoPtr CModulePtr;

////////////////////////////////////////////////////
inline TModuleId const& CModule::GetId() const
{
	return m_Id;
}

#endif //_MODULE_H_
