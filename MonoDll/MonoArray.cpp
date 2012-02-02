#include "StdAfx.h"
#include "MonoArray.h"

#include "MonoObject.h"

CMonoArray::CMonoArray(int size)
	: curIndex(0)
{
	if(size<1)
		gEnv->pLog->LogError("Attempted to create array with invalid size %i", size);
	//	delete this;

	m_pArray = (mono::array)mono_array_new(mono_domain_get(), mono_get_object_class(), size);
}

IMonoObject *CMonoArray::GetItem(int index)
{ 
	return *(mono::object)mono_array_get((MonoArray *)m_pArray, MonoObject *, index);
}

void CMonoArray::InsertObject(mono::object object)
{
	mono_array_set((MonoArray *)m_pArray, MonoObject *, curIndex, (MonoObject *)object);

	curIndex++;
}

void CMonoArray::InsertString(mono::string pString)
{
	mono_array_set((MonoArray *)m_pArray, MonoString *, curIndex, (MonoString *)pString);

	curIndex++;
}

void CMonoArray::InsertArray(mono::array arr)
{
	mono_array_set((MonoArray *)m_pArray, MonoArray *, curIndex, (MonoArray *)arr);

	curIndex++;
}

void CMonoArray::Insert(IMonoObject *pObject) 
{ 
	InsertObject(static_cast<CMonoObject *>(pObject)->GetMonoObject()); 
}

void CMonoArray::Insert(MonoAnyValue value)
{ 
	if(value.type==MONOTYPE_STRING)
		InsertString(ToMonoString(value.str));
	else
		Insert(gEnv->pMonoScriptSystem->GetConverter()->CreateObject(value)); 
}