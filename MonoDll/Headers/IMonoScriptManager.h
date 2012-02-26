/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// IMonoScriptManager interface for external projects, i.e. CryGame.
//////////////////////////////////////////////////////////////////////////
// 25/02/2011 : Created by Filip 'i59' Lundgren (Based on version by ins\)
////////////////////////////////////////////////////////////////////////*/
#ifndef __I_MONO_SCRIPT_MANAGER_H__
#define __I_MONO_SCRIPT_MANAGER_H__

#include <IGameObject.h>

struct IMonoArray;
struct IMonoClass;

enum EMonoScriptType
{
	EMonoScriptType_NULL = -1,

	EMonoScriptType_GameRules,
	EMonoScriptType_FlowNode,
	EMonoScriptType_StaticEntity,
	EMonoScriptType_Entity,
	EMonoScriptType_Actor,
	EMonoScriptType_EditorForm,
	EMonoScriptType_Unknown,
};

struct IMonoScriptManager : public IGameObjectExtension
{
	/// <summary>
	/// Instantiates a script (with constructor parameters if supplied) of type and name
	/// This assumes that the script was present in a .dll in Plugins or within a .cs file when PostInit was called.
	/// </summary>
	virtual int InstantiateScript(EMonoScriptType scriptType, const char *scriptName, IMonoArray *pConstructorParameters = nullptr) = 0;
	/// <summary>
	/// Gets the instantied script with the supplied id.
	/// </summary>
	virtual IMonoClass *GetScriptById(int id) = 0;
	/// <summary>
	/// Removes and destructs an instantiated script with the supplied id if found.
	/// </summary>
	virtual void RemoveScriptInstance(int id) = 0;
};

#endif //__I_MONO_SCRIPT_MANAGER_H__