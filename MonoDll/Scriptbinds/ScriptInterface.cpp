#include "StdAfx.h"
#include "ScriptInterface.h"

#include <IEntitySystem.h>

CScriptbind_ScriptInterface::CScriptbind_ScriptInterface()
{
	REGISTER_METHOD(GetScriptTable);

	REGISTER_METHOD(CallMethod);
	REGISTER_METHOD(CallMethodVoid);
}

IScriptTable *CScriptbind_ScriptInterface::GetScriptTable(EntityId entityId)
{
	IEntity *pEntity = gEnv->pEntitySystem->GetEntity(entityId);
	if(pEntity)
		return pEntity->GetScriptTable();

	return NULL;
}

mono::object CScriptbind_ScriptInterface::CallMethod(IScriptTable *pScriptTable, mono::string methodName, ELuaVariableType returnType, mono::object args)
{
	/*HSCRIPTFUNCTION pfnScriptFunction = 0;
	if(pScriptTable->GetValue(ToCryString(methodName), pfnScriptFunction))
	{
		void *result = NULL;
		if(gEnv->pScriptSystem->BeginCall(pfnScriptFunction))
		{
			if(IMonoArray *pArgs = *args)
			{
				//for(int i = 0; i < pArgs->GetSize(); i++)
					//gEnv->pScriptSystem->PushFuncParam(pArgs->GetItem(i)->GetAnyValue().GetValue());
			}

			gEnv->pScriptSystem->EndCall(result);
			gEnv->pScriptSystem->ReleaseFunc(pfnScriptFunction);
		}

		Script::CallReturn(gEnv->pScriptSystem, pfnScriptFunction, pScriptTable, result);
	}*/

	return NULL;
}

void CScriptbind_ScriptInterface::CallMethodVoid(IScriptTable *pScriptTable, mono::string funcName, mono::object args)
{
	const char *methodName = ToCryString(funcName);

	if(pScriptTable->GetValueType(methodName) == svtFunction)
	{
		if(gEnv->pScriptSystem->BeginCall(pScriptTable, methodName))
		{
			if(IMonoArray *pArgs = *args)
			{
				//for(int i = 0; i < pArgs->GetSize(); i++)
					//gEnv->pScriptSystem->PushFuncParam(pArgs->GetItem(i)->GetAnyValue().GetValue());
			}

			gEnv->pScriptSystem->EndCall();
		}
	}
}