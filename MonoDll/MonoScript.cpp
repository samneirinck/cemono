#include "StdAfx.h"
#include "MonoScript.h"

#include "MonoScriptSystem.h"

#include "MonoClass.h"
#include "MonoString.h"
#include "MonoArray.h"
#include "MonoObject.h"

CMonoScript::CMonoScript(int _id, const char *_name, EMonoScriptType _type)
	: m_id(_id)
	, m_name(_name)
	, m_type(_type)
{
}

CMonoScript::~CMonoScript()
{
	//static_cast<CMonoScriptSystem *>(gEnv->pMonoScriptSystem)->UnregisterScriptInstance(m_type, m_name);
}

void CMonoScript::Release()
{
	//static_cast<CMonoScriptSystem *>(gEnv->pMonoScriptSystem)->RemoveScriptInstance(m_type, m_name);

	delete this;
}

IMonoObject *CMonoScript::InvokeMethod(const char *func, IMonoArray *pArgs)
{
	IMonoClass *pManagerClass = static_cast<CMonoScriptSystem *>(gEnv->pMonoScriptSystem)->GetManagerClass();

	if(pArgs)
	{
		CMonoArray args(3);
		args.InsertObject(CreateMonoObject<int>(m_id));
		args.InsertString(func);
		args.InsertArray(pArgs);
		return pManagerClass->CallMethod("InvokeScriptFunctionById", &args);
	}
	else
	{
		CMonoArray args(2);
		args.InsertObject(CreateMonoObject<int>(m_id));
		args.InsertString(func);
		return pManagerClass->CallMethod("InvokeScriptFunctionById", &args);
	}

	return NULL;
}