/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// Scriptbind used to supply a bridge between lua and mono scripts,
// to avoid having to rewrite all existing code right away.
//////////////////////////////////////////////////////////////////////////
// 26/03/2012 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __SCRIPTBIND_SCRIPTINTERFACE__
#define __SCRIPTBIND_SCRIPTINTERFACE__

#include <MonoCommon.h>
#include <IMonoScriptBind.h>

struct IScriptTable;

enum ELuaVariableType
{
	eLVT_Boolean,
	eLVT_Integer,
	eLVT_Float,
	eLVT_String
};

class CScriptbind_ScriptInterface : public IMonoScriptBind
{
public:
	CScriptbind_ScriptInterface();
	~CScriptbind_ScriptInterface() {}

	// IMonoScriptBind
	virtual const char *GetNamespace() override { return "CryEngine.Lua"; }
	virtual const char *GetClassName() override { return "ScriptTable"; }
	// ~IMonoScriptBind

	// Externals
	static IScriptTable *GetScriptTable(EntityId entity);

	static mono::object CallMethod(IScriptTable *pScriptTable, mono::string methodName, ELuaVariableType returnType, mono::array args);
	static void CallMethodVoid(IScriptTable *pScriptTable, mono::string methodName, mono::array args);
	// ~Externals
};

#endif //__SCRIPTBIND_PHYSICALWORLD__