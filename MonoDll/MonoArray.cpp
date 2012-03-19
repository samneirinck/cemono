#include "StdAfx.h"
#include "MonoArray.h"

#include "MonoObject.h"

CScriptArray::CScriptArray(int size)
	: curIndex(0)
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
	curIndex = 0;
	m_pArray = 0;

	mono_gchandle_free(m_arrayHandle); 
}

IMonoObject *CScriptArray::GetItem(int index)
{ 
	return *(mono::object)mono_array_get((MonoArray *)m_pArray, MonoObject *, index);
}

void CScriptArray::InsertObject(mono::object object)
{
	mono_array_set((MonoArray *)m_pArray, MonoObject *, curIndex, (MonoObject *)object);

	curIndex++;
}

void CScriptArray::InsertString(mono::string string)
{
	mono_array_set((MonoArray *)m_pArray, MonoString *, curIndex, (MonoString *)string);

	curIndex++;
}

void CScriptArray::InsertArray(mono::array arr)
{
	mono_array_set((MonoArray *)m_pArray, MonoArray *, curIndex, (MonoArray *)arr);

	curIndex++;
}

void CScriptArray::Insert(IMonoObject *pObject) 
{ 
	InsertObject(static_cast<CScriptObject *>(pObject)->GetMonoObject()); 
}

void CScriptArray::Insert(MonoAnyValue value)
{ 
	if(value.type==MONOTYPE_STRING)
		InsertString(ToMonoString(value.str));
	else
		Insert(gEnv->pMonoScriptSystem->GetConverter()->CreateObject(value)); 
}