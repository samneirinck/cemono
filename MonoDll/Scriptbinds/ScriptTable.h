/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// IActionListener mono extension
//////////////////////////////////////////////////////////////////////////
// 02/09/2012 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/

#include <IMonoScriptBind.h>

#include "MonoCommon.h"

class CScriptbind_ScriptTable 
	: public IMonoScriptBind
{
public:
	CScriptbind_ScriptTable();
	~CScriptbind_ScriptTable() {}

	// IMonoScriptBind
	virtual const char *GetClassName() { return "NativeScriptTableMethods"; }
	// ~IMonoScriptBind

private:
	static IScriptTable *GetScriptTable(IEntity *pEntity);
};
