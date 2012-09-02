#include "stdafx.h"
#include "ScriptTable.h"

#include <IEntitySystem.h>

CScriptbind_ScriptTable::CScriptbind_ScriptTable()
{
	REGISTER_METHOD(GetScriptTable);
}

IScriptTable *CScriptbind_ScriptTable::GetScriptTable(IEntity *pEntity)
{
	return pEntity->GetScriptTable();
}