/////////////////////////////////////////////////////////////////
// Copyright (C), RenEvo Software & Designs, 2008
// FGPlugin Source File
//
// Module.cpp
//
// Purpose: Module container
//
// History:
//	- 8/03/08 : File created - KAK
/////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Module.h"
#include "FlowModuleNodes.h"
#include "Game.h"
#include "ILevelSystem.h"

////////////////////////////////////////////////////
CModule::CModule()
{
	m_pRootGraph = NULL;
	m_Id = MODULEID_INVALID;
}

////////////////////////////////////////////////////
CModule::~CModule()
{
	Destroy();
}

////////////////////////////////////////////////////
bool CModule::Init(char const* module, TModuleId const& id)
{
	bool bResult = (m_pRootGraph != NULL);

	if (!m_pRootGraph)
	{
		IFlowSystem *pSystem = gEnv->pFlowSystem;
		assert(pSystem);

		// Find module file
		XmlNodeRef moduleRef;
		if (LoadModuleFile(module, moduleRef))
		{
			// Create graph
			m_pRootGraph = pSystem->CreateFlowGraph();
			if (m_pRootGraph)
			{
				// Node map to convert stored Ids to made Ids
				typedef std::map<TFlowNodeId, TFlowNodeId> TNodeMap;
				TNodeMap nodeMap;

				// Create nodes
				XmlNodeRef nodesRef = moduleRef->findChild("Nodes");
				if (nodesRef)
				{
					int nodeChildCount = nodesRef->getChildCount();
					XmlNodeRef nodeRef = NULL;
					for (int nodeNum = 0; nodeNum < nodeChildCount; ++nodeNum)
					{
						nodeRef = nodesRef->getChild(nodeNum);
						if (!nodeRef || strcmp(nodeRef->getTag(), "Node"))
						{
							continue;
						}

						TFlowNodeId storedId = InvalidFlowNodeId;
						if (nodeRef->getAttr("Id", storedId) && InvalidFlowNodeId != storedId)
						{
							// Create node based on class
							string sClass = nodeRef->getAttr("Class");
							TFlowNodeTypeId typeId = pSystem->GetTypeId(sClass.c_str());
							if (InvalidFlowNodeTypeId == typeId)
							{
								Warning(module, "Invalid Class - \'%s\' (Node = %d)", sClass.c_str(), storedId);
							}
							else
							{
								string sName;
								sName.Format("%s%d", sClass.c_str(), storedId);
								TFlowNodeId madeId = m_pRootGraph->CreateNode(typeId, sName.c_str());
								if (InvalidFlowNodeId == madeId)
								{
									Warning(module, "Failed to create Node of Class \'%s\' (Node = %d)", sClass.c_str(), storedId);
								}
								else
								{
									// Set flags
									uint32 savedFlags = 0;
									if (nodeRef->getAttr("flags", savedFlags))
									{
										SFlowNodeConfig config;
										m_pRootGraph->GetNodeConfiguration(madeId, config);
										config.nFlags &= (~EFLN_CORE_MASK);
										config.nFlags |= savedFlags;
									}

									// Set Input values
									XmlNodeRef inputsRef = nodeRef->findChild("Inputs");
									if (inputsRef)
									{
										int inputCount = inputsRef->getNumAttributes();
										for (int inputNum = 0; inputNum < inputCount; ++inputNum)
										{
											SFlowAddress addr;
											char const* key = {0,};
											char const* value = {0,};
											if (!inputsRef->getAttributeByIndex(inputNum, &key, &value) ||
												!ResolveAddress(addr, m_pRootGraph, madeId, key))
											{
												Warning(module, "Bad resolve of Port \'%s\' on Node %d", key, madeId);
											}
											else if (!m_pRootGraph->SetInputValue(addr.node, addr.port, TFlowInputData(string(value))))
											{
												Warning(module, "Failed to set input value on Port \'%s\' on Node %d", key, madeId);
											}
										}
									}

									// Store for edge creation later
									nodeMap[storedId] = madeId;
								}
							}
						}
					}
				}

				// Create edges
				XmlNodeRef edgesRef = moduleRef->findChild("Edges");
				if (edgesRef)
				{
					int edgeChildCount = edgesRef->getChildCount();
					XmlNodeRef edgeRef = NULL;
					for (int edgeNum = 0; edgeNum < edgeChildCount; ++edgeNum)
					{
						edgeRef = edgesRef->getChild(edgeNum);
						if (!edgeRef || strcmp(edgeRef->getTag(), "Edge"))
						{
							continue;
						}

						// Skip if disabled
						bool bEnabled = false;
						if (!edgeRef->getAttr("enabled", bEnabled) || !bEnabled)
						{
							continue;
						}

						// Look up made nodes
						TFlowNodeId storedInId = InvalidFlowNodeId;
						TFlowNodeId storedOutId = InvalidFlowNodeId;
						if (edgeRef->getAttr("nodeIn", storedInId) && 
							edgeRef->getAttr("nodeOut", storedOutId) &&
							InvalidFlowNodeId != storedInId &&
							InvalidFlowNodeId != storedOutId)
						{
							// Convert (no making out jokes please!)
							TFlowNodeId madeInId = InvalidFlowNodeId;
							TFlowNodeId madeOutId = InvalidFlowNodeId;
							TNodeMap::iterator convertIn = nodeMap.find(storedInId);
							TNodeMap::iterator convertOut = nodeMap.find(storedOutId);
							if (convertIn != nodeMap.end()) madeInId = convertIn->second;
							if (convertOut != nodeMap.end()) madeOutId = convertOut->second;
							if (InvalidFlowNodeId == madeInId || InvalidFlowNodeId == madeOutId)
							{
								Warning(module, "Bad conversion of Nodes when constructing Edge (In = %d, Out = %d)", storedInId, storedOutId);
							}
							else
							{
								// Resolve addresses
								SFlowAddress addrIn, addrOut;
								char const* portIn = edgeRef->getAttr("portIn");
								char const* portOut = edgeRef->getAttr("portOut");
								if (!ResolveAddress(addrIn, m_pRootGraph, madeInId, portIn, false) ||
									!ResolveAddress(addrOut, m_pRootGraph, madeOutId, portOut, true))
								{
									Warning(module, "Bad resolve of Edge - In: \'%s\' [%d] - Out: \'%s\' [%d]", portIn, madeInId, portOut, madeOutId);
								}
								else if (!m_pRootGraph->LinkNodes(addrOut, addrIn))
								{
									Warning(module, "Failed to create Edge - In: \'%s\' [%d] - Out: \'%s\' [%d]", portIn, madeInId, portOut, madeOutId);
								}
							}
						}
					}
				}

				// Root graph is for cloning, so not active!
				m_pRootGraph->SetEnabled(false);
				m_pRootGraph->SetActive(false);
				bResult = true;
				m_Id = id;
			}
		}
	}

	return bResult;
}

////////////////////////////////////////////////////
void CModule::Destroy()
{
	m_pRootGraph = NULL;

	TInstanceList::iterator i = m_instances.begin();
	TInstanceList::iterator end = m_instances.end();
	for (; i != end; ++i)
	{
		i->pGraph = NULL;
	}
	m_instances.clear();
}

////////////////////////////////////////////////////
bool CModule::CreateInstance(MODULE_FLOWNODE_CALLERNODE* pCaller, EntityId const& entityId, TModuleParams const& params)
{
	bool bResult = false;

	if (m_pRootGraph)
	{
		IFlowSystem *pSystem = gEnv->pFlowSystem;
		assert(pSystem);

		// Find unused instance
		/*SInstance *pInstance = NULL;
		TInstanceList::iterator i = m_instances.begin();
		TInstanceList::iterator end = m_instances.end();
		for (; i != end; ++i)
		{
			if (!i->bUsed)
			{
				pInstance = &(*i);
				break;
			}
		}*/

		// Clone and create instance
		IFlowGraphPtr pClone = pSystem->CreateFlowGraph();
		if (pClone)
		{
			// Node map to convert  aster Ids to clone Ids
			typedef std::map<TFlowNodeId, TFlowNodeId> TNodeMap;
			TNodeMap nodeMap;

			// Clone nodes
			IFlowNodeIteratorPtr pNodeIter = m_pRootGraph->CreateNodeIterator();
			if (pNodeIter)
			{
				TFlowNodeId masterId = InvalidFlowNodeId;
				IFlowNodeData *pMasterData;
				while (pMasterData = pNodeIter->Next(masterId))
				{
					// Create clone
					TFlowNodeId cloneId = pClone->CreateNode(pMasterData->GetNodeTypeId(), pMasterData->GetNodeName());
					if (InvalidFlowNodeId == cloneId)
					{
						string s; s.Format("Clone %d", m_Id);
						Warning(s.c_str(), "Failed to clone Node \'%s\'", pMasterData->GetNodeName());
					}
					else
					{
						// Copy config
						SFlowNodeConfig masterConfig, cloneConfig;
						m_pRootGraph->GetNodeConfiguration(masterId, masterConfig);
						pClone->GetNodeConfiguration(cloneId, cloneConfig);
						cloneConfig.nFlags = masterConfig.nFlags;

						// Copy input values
						SInputPortConfig const* pMasterInput = masterConfig.pInputPorts;
						int nInputCount = 0;
						while (pMasterInput && pMasterInput->name)
						{
							pClone->SetInputValue(cloneId, nInputCount, *(m_pRootGraph->GetInputValue(masterId, nInputCount)));

							// Advance
							nInputCount++;
							pMasterInput++;
						}

						// Set Entity last
						pClone->SetEntityId(cloneId, entityId);

						nodeMap[masterId] = cloneId;
					}
				}
			}

			// Clone edges
			IFlowEdgeIteratorPtr pEdgeIter = m_pRootGraph->CreateEdgeIterator();
			if (pEdgeIter)
			{
				IFlowEdgeIterator::Edge masterEdge;
				SFlowAddress from, to;
				while (pEdgeIter->Next(masterEdge))
				{
					from.isOutput = true;
					from.node = nodeMap[masterEdge.fromNodeId];
					from.port = masterEdge.fromPortId;
					to.isOutput = false;
					to.node = nodeMap[masterEdge.toNodeId];
					to.port = masterEdge.toPortId;
					if (!pClone->LinkNodes(from, to))
					{
						string s; s.Format("Clone %d", m_Id);
						Warning(s.c_str(), "Failed to clone Edge - In: \'%d\' [%d] - Out: \'%d\' [%d]", to.port, to.node, from.port, from.node);
					}
				}
			}

			// Start up
			pClone->SetGraphEntity(entityId);
			pClone->SetEnabled(true);
			pClone->SetActive(true);
			pClone->InitializeValues();

			// Store instance
			SInstance instance;
			instance.pGraph = pClone;
			instance.pCaller = pCaller;
			instance.bUsed = true;
			m_instances.push_back(instance);

			// Call start node and pass in params
			ActivateGraph(pClone, entityId, params);
			bResult = true;
		}
	}

	return bResult;
}

////////////////////////////////////////////////////
bool CModule::DestroyInstance(IFlowGraph *pInstance, bool bSuccess, TModuleParams const& params)
{
	bool bResult = false;

	// Find the instance
	TInstanceList::iterator i = m_instances.begin();
	TInstanceList::iterator end = m_instances.end();
	for (; i != end; ++i)
	{
		if (i->pGraph == pInstance)
		{
			DeactivateGraph(*i, bSuccess, params);

			// Turn it off
			i->bUsed = false;
			break;
		}
	}

	return bResult;
}

////////////////////////////////////////////////////
bool CModule::LoadModuleFile(char const* module, XmlNodeRef &ref) const
{
	ref = NULL;

	string szModuleFile = module;
	if (szModuleFile.substr(szModuleFile.size()-4,4) != ".xml")
	{
		szModuleFile += ".xml";
	}

	// 1. Level folder
	string szLevelPath;
	if(gEnv->bEditor)
	{
		char *levelName;
		char *levelPath;
		g_pGame->GetIGameFramework()->GetEditorLevel(&levelName, &levelPath);
		szLevelPath = levelPath;
	}
	else
	{
		ILevel *pLevel = g_pGame->GetIGameFramework()->GetILevelSystem()->GetCurrentLevel();
		if (pLevel)
		{
			szLevelPath = pLevel->GetLevelInfo()->GetPath();
		}
	}
	szLevelPath += MODULE_FOLDER_NAME;
	szLevelPath += szModuleFile;
	ref = gEnv->pSystem->LoadXmlFile(szLevelPath.c_str());
	if (!ref)
	{
		// 2. Master folder
		string szModPath = gEnv->pCryPak->GetModDir();
		szModPath += MODULE_FOLDER_NAME;
		szModPath += szModuleFile;
		ref = gEnv->pSystem->LoadXmlFile(szModPath.c_str());
	}

	return (ref != NULL);
}

////////////////////////////////////////////////////
bool CModule::ResolveAddress(SFlowAddress &addr, IFlowGraph *pGraph, TFlowNodeId nodeId, char const* portName, bool bIsOutput) const
{
	// Reset
	addr.node = nodeId;
	addr.port = InvalidFlowPortId;
	addr.isOutput = bIsOutput;

	if (pGraph)
	{
		// Get config
		SFlowNodeConfig config;
		pGraph->GetNodeConfiguration(nodeId, config);
		if (!bIsOutput && config.pInputPorts)
		{
			SInputPortConfig const* pInput = config.pInputPorts;
			if (pInput)
			{
				int count = 0;
				do
				{
					if (!strcmp(pInput->name, portName))
					{
						addr.port = count;
						break;
					}

					// Advance
					count++;
					pInput++;
				} while (pInput);
			}
		}
		else if (bIsOutput && config.pOutputPorts)
		{
			SOutputPortConfig const* pOutput = config.pOutputPorts;
			if (pOutput)
			{
				int count = 0;
				do
				{
					if (!strcmp(pOutput->name, portName))
					{
						addr.port = count;
						break;
					}

					// Advance
					count++;
					pOutput++;
				} while (pOutput);
			}
		}
	}

	return (addr.port != InvalidFlowPortId);
}

////////////////////////////////////////////////////
void CModule::Warning(char const* file, char const* msg, ...) const
{
	va_list argptr;
	va_start(argptr, msg);

	string OutputMsg;
	OutputMsg.Format("[Module] Warning [\'%s\']: %s", file, msg);

	gEnv->pLog->LogV(IMiniLog::eAlways, OutputMsg.c_str(), argptr);
}

////////////////////////////////////////////////////
void CModule::ActivateGraph(IFlowGraph *pGraph, EntityId const& entityId, TModuleParams const& params) const
{
	if (pGraph)
	{
		IFlowNodeIteratorPtr pNodeIter = pGraph->CreateNodeIterator();
		if (pNodeIter)
		{
			static size_t startLen = strlen(MODULE_FLOWNODE_START);
			static size_t returnLen = strlen(MODULE_FLOWNODE_RETURN);
			TFlowNodeId id = InvalidFlowNodeId;
			IFlowNodeData *pData;
			while (pData = pNodeIter->Next(id))
			{
				// Check if its the starting node
				if (!strncmp(pData->GetNodeName(), MODULE_FLOWNODE_START, startLen))
				{
					MODULE_FLOWNODE_STARTNODE* pNode = (MODULE_FLOWNODE_STARTNODE*)pData->GetNode();
					pNode->OnActivate(entityId, params);
				}

				// Check if its the returning node
				if (!strncmp(pData->GetNodeName(), MODULE_FLOWNODE_RETURN, returnLen))
				{
					MODULE_FLOWNODE_RETURNNODE* pNode = (MODULE_FLOWNODE_RETURNNODE*)pData->GetNode();
					pNode->SetModuleId(m_Id);
				}
			}
		}
	}
}

////////////////////////////////////////////////////
void CModule::DeactivateGraph(SInstance &instance, bool bSuccess, TModuleParams const& params) const
{
	IFlowGraph *pGraph = instance.pGraph;
	if (pGraph)
	{
		pGraph->SetEnabled(false);
		pGraph->SetActive(false);

		// Deactivate all nodes
		IFlowNodeIteratorPtr pNodeIter = pGraph->CreateNodeIterator();
		if (pNodeIter)
		{
			TFlowNodeId id = InvalidFlowNodeId;
			while (pNodeIter->Next(id))
			{
				pGraph->SetRegularlyUpdated(id, false);
			}
		}
	}

	if (instance.pCaller)
	{
		// Notify
		instance.pCaller->OnReturn(bSuccess, params);
	}
}
