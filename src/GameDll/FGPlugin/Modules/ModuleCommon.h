/////////////////////////////////////////////////////////////////
// Copyright (C), RenEvo Software & Designs, 2008
// FGPlugin Source File
//
// ModuleCommon.h
//
// Purpose: Common declarations shared across Module setup
//
// History:
//	- 8/03/08 : File created - KAK
/////////////////////////////////////////////////////////////////

#ifndef _MODULECOMMON_H_
#define _MODULECOMMON_H_

#include <IFlowSystem.h>

// Node names
#define MODULE_FLOWNODE_START		"Module:Start"
#define MODULE_FLOWNODE_RETURN		"Module:Return"
#define MODULE_FLOWNODE_CALLER		"Module:Call"

// Node classes
#define MODULE_FLOWNODE_STARTNODE	CFlowNode_ModuleLoaded
#define MODULE_FLOWNODE_RETURNNODE	CFlowNode_ModuleReturn
#define MODULE_FLOWNODE_CALLERNODE	CFlowNode_CallModule

// Setup
#define MODULE_FOLDER_NAME ("\\Modules\\")

// Parameter passing object
#define MODULE_MAX_PARAMS (6)
typedef std::vector<TFlowInputData> TModuleParams;

// Module Ids
#define MODULEID_INVALID (0)
typedef uint TModuleId;

class MODULE_FLOWNODE_STARTNODE;
class MODULE_FLOWNODE_RETURNNODE;
class MODULE_FLOWNODE_CALLERNODE;

#endif //_MODULECOMMON_H_
