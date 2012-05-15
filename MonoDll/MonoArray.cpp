#include "StdAfx.h"
#include "MonoArray.h"

#include "MonoObject.h"

CScriptArray::CScriptArray(int size)
	: m_curIndex(0)
{
	if(size<1)
	{
		gEnv->pLog->LogError("Attempted to create array with invalid size %i", size);
		Release();
	}

	m_pArray = (mono::array)mono_array_new(mono_domain_get(), mono_get_object_class(), size);
}

CScriptArray::~CScriptArray()
{
	m_curIndex = 0;
	m_pArray = 0;

	mono_gchandle_free(m_arrayHandle); 
}

IMonoObject *CScriptArray::GetItem(int index)
{ 
	if(index < GetSize())
	{
		if(mono::object monoObj = (mono::object)mono_array_get((MonoArray *)m_pArray, MonoObject *, index))
			return *monoObj;
	}
	else
		CryLogAlways("[Warning] Index out of range exception: Attempted to access index %i on IMonoArray of size %i", index, GetSize());

	return NULL;
}

void CScriptArray::InsertObject(mono::object object)
{
	if(m_curIndex >= GetSize())
	{
		CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_WARNING, "Attempted to insert too many objects into array of size %i", GetSize());
		return;
	}

	mono_array_set((MonoArray *)m_pArray, MonoObject *, m_curIndex, (MonoObject *)object);

	m_curIndex++;
}

void CScriptArray::InsertString(mono::string string)
{
	if(m_curIndex >= GetSize())
	{
		CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_WARNING, "Attempted to insert too many objects into array of size %i", GetSize());
		return;
	}

	mono_array_set((MonoArray *)m_pArray, MonoString *, m_curIndex, (MonoString *)string);

	m_curIndex++;
}

void CScriptArray::InsertArray(mono::array arr)
{
	if(m_curIndex >= GetSize())
	{
		CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_WARNING, "Attempted to insert too many objects into array of size %i", GetSize());
		return;
	}

	mono_array_set((MonoArray *)m_pArray, MonoArray *, m_curIndex, (MonoArray *)arr);

	m_curIndex++;
}

void CScriptArray::Insert(IMonoObject *pObject) 
{ 
	InsertObject(pObject->GetMonoObject()); 
}

void CScriptArray::Insert(MonoAnyValue value)
{ 
	if(value.type==eMonoAnyType_String)
		InsertString(ToMonoString(value.str));
	else
		Insert(gEnv->pMonoScriptSystem->GetConverter()->CreateObject(value)); 
}