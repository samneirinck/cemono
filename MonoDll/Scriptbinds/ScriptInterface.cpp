#include "StdAfx.h"
#include "ScriptInterface.h"

#include <IEntitySystem.h>

CScriptbind_ScriptInterface::TScriptTables CScriptbind_ScriptInterface::m_scriptTables = CScriptbind_ScriptInterface::TScriptTables();

CScriptbind_ScriptInterface::CScriptbind_ScriptInterface()
{
	REGISTER_METHOD(GetScriptTable);
	REGISTER_METHOD(InvokeMethod);
}

int CScriptbind_ScriptInterface::GetScriptTable(EntityId entityId)
{
	IEntity *pEntity = gEnv->pEntitySystem->GetEntity(entityId);
	if(pEntity)
	{
		IScriptTable *pScriptTable = pEntity->GetScriptTable();
		if(!pScriptTable)
			return -1;

		for each(auto scriptTable in m_scriptTables)
		{
			if(scriptTable.second == pScriptTable)
				return scriptTable.first;
		}

		m_scriptTables.insert(TScriptTables::value_type(m_scriptTables.size(), pScriptTable));
		return m_scriptTables.size() - 1;
	}

	return -1;
}

mono::object CScriptbind_ScriptInterface::InvokeMethod(int scriptTable, mono::string methodName, mono::array args)
{/*
	IScriptTable *pScriptTable = m_scriptTables[scriptTable];

	HSCRIPTFUNCTION pfnScriptFunction = 0;
	if(pScriptTable && pScriptTable->GetValue(ToCryString(methodName), pfnScriptFunction))
	{
		void *result = NULL;
		if(gEnv->pScriptSystem->BeginCall(pfnScriptFunction))
		{
			IMonoArray *pArgs = *args;
			if(pArgs)
			{
				for(int i = 0; i < pArgs->GetSize(); i++)
					gEnv->pScriptSystem->PushFuncParam(pArgs->GetItem(i)->Unbox<void *>());
			}

			gEnv->pScriptSystem->EndCall(result);
			gEnv->pScriptSystem->ReleaseFunc(pfnScriptFunction);
		}

		Script::CallReturn(gEnv->pScriptSystem, pfnScriptFunction, pScriptTable, result);
	}*/

	return NULL;
}