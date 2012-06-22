#include "StdAfx.h"
#include "MonoArray.h"

#include "MonoObject.h"

CScriptArray::CScriptArray(mono::object managedArray)
{
	CRY_ASSERT(managedArray);

	m_pObject = (MonoObject *)managedArray;

	m_objectHandle = mono_gchandle_new(m_pObject, false);
}

CScriptArray::CScriptArray(int size)
	: m_curIndex(0)
{
	CRY_ASSERT(size > 0);

	m_pObject = (MonoObject *)mono_array_new(mono_domain_get(), mono_get_object_class(), size);

	m_objectHandle = mono_gchandle_new(m_pObject, false);
}

CScriptArray::~CScriptArray()
{
	m_curIndex = 0;
}

void CScriptArray::Resize(int size)
{
	MonoArray *pOldArray = (MonoArray *)m_pObject;
	int oldArraySize = mono_array_length(pOldArray);

	m_pObject = (MonoObject *)mono_array_new(mono_domain_get(), mono_get_object_class(), size);

	for(int i = 0; i < size; i++)
	{
		if(i > oldArraySize)
			mono_array_set((MonoArray *)m_pObject, MonoObject *, i, mono_array_get(pOldArray, MonoObject *, i));
	}
}

IMonoObject *CScriptArray::GetItem(int index)
{ 
	CRY_ASSERT(index <= GetSize());

	if(mono::object monoObj = (mono::object)mono_array_get((MonoArray *)m_pObject, MonoObject *, index))
		return *monoObj;

	return NULL;
}

void CScriptArray::InsertMonoObject(mono::object object, int index)
{
	CRY_ASSERT((index == -1 ? m_curIndex : index) < GetSize());

	mono_array_set((MonoArray *)m_pObject, MonoObject *, m_curIndex, (MonoObject *)object);

	m_curIndex++;
}

void CScriptArray::InsertMonoString(mono::string string, int index)
{
	CRY_ASSERT((index == -1 ? m_curIndex : index) < GetSize());

	mono_array_set((MonoArray *)m_pObject, MonoString *, index != -1 ? index : m_curIndex, (MonoString *)string);

	m_curIndex++;
}

void CScriptArray::InsertMonoArray(mono::object arr, int index)
{
	CRY_ASSERT((index == -1 ? m_curIndex : index) < GetSize());

	mono_array_set((MonoArray *)m_pObject, MonoArray *, index != -1 ? index : m_curIndex, (MonoArray *)arr);

	m_curIndex++;
}

void CScriptArray::InsertNativePointer(void *ptr, int index)
{ 
	CRY_ASSERT((index == -1 ? m_curIndex : index) < GetSize());

	mono_array_set((MonoArray *)m_pObject, void *, index != -1 ? index : m_curIndex, ptr);

	m_curIndex++;
}

void CScriptArray::InsertObject(IMonoObject *pObject, int index)
{
	if(!pObject)
	{
		InsertMonoObject(NULL, index);
		return;
	}

	if(pObject->GetType() == eMonoAnyType_Array)
		InsertMonoArray(pObject->GetManagedObject(), index);
	else
		InsertMonoObject(pObject->GetManagedObject(), index); 
}

void CScriptArray::InsertAny(MonoAnyValue value, int index)
{ 
	if(value.type==eMonoAnyType_String)
		InsertMonoString(ToMonoString(value.str), index);
	else
		Insert(gEnv->pMonoScriptSystem->GetConverter()->CreateObject(value), index);
}