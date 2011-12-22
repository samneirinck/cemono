#include "StdAfx.h"
#include "MonoArray.h"

#include "MonoObject.h"

CMonoArray::CMonoArray(int size)
	: curIndex(0)
{
	if(size<1)
		delete this;

	m_pArray = mono_array_new(mono_domain_get(), mono_get_object_class(), size);
}

IMonoObject *CMonoArray::GetItem(int index)
{ 
	return new CMonoObject(mono_array_get(m_pArray, MonoObject *, index));
}

void CMonoArray::InsertObject(MonoObject *pObject)
{
	mono_array_set(m_pArray, MonoObject *, curIndex, pObject);

	curIndex++;
}

void CMonoArray::InsertString(MonoString *pString)
{
	mono_array_set(m_pArray, MonoString *, curIndex, pString);

	curIndex++;
}

void CMonoArray::InsertArray(MonoArray *pArray)
{
	mono_array_set(m_pArray, MonoArray *, curIndex, pArray);

	curIndex++;
}

void CMonoArray::InsertObject(IMonoObject *pObject) 
{ 
	InsertObject(static_cast<CMonoObject *>(pObject)->GetMonoObject()); 
}