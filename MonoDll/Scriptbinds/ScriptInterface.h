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

class CScriptbind_ScriptInterface : public IMonoScriptBind
{
	typedef std::map<int, IScriptTable *> TScriptTables;
public:
	CScriptbind_ScriptInterface();
	~CScriptbind_ScriptInterface() {}

	// IMonoScriptBind
	virtual const char *GetClassName() { return "LuaScriptTable"; }
	// ~IMonoScriptBind

	// Externals
	static int GetScriptTable(EntityId entity);

	static mono::object InvokeMethod(int scriptTable, mono::string methodName, mono::array args);
	// ~Externals

	static TScriptTables m_scriptTables;
};

#endif //__SCRIPTBIND_PHYSICALWORLD__