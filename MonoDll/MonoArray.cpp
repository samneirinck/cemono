#include "StdAfx.h"
#include "MonoArray.h"

#include "MonoDomain.h"
#include "MonoObject.h"

#include "MonoScriptSystem.h"

#include <IMonoClass.h>

MonoClass *CScriptArray::m_pDefaultElementClass = NULL;

CScriptArray::CScriptArray(mono::object managedArray)
{
	CRY_ASSERT(managedArray);

	m_pObject = (MonoObject *)managedArray;
	m_pElementClass = mono_class_get_element_class(GetMonoClass());

	m_objectHandle = mono_gchandle_new(m_pObject, false);
	m_pClass = NULL;
}

CScriptArray::CScriptArray(int size, IMonoClass *pContainingType)
	: m_lastIndex(0)
{
	CRY_ASSERT(size > 0);

	m_pElementClass = (pContainingType ? (MonoClass *)(pContainingType)->GetManagedObject() : m_pDefaultElementClass);
	CRY_ASSERT(m_pElementClass);

	m_pObject = (MonoObject *)mono_array_new(mono_domain_get(), m_pElementClass, size);

	m_objectHandle = mono_gchandle_new(m_pObject, false);
	m_pClass = NULL;
}

CScriptArray::~CScriptArray()
{
	m_lastIndex = 0;
}

void CScriptArray::Resize(int size)
{
	int oldArraySize = GetSize();
	if(oldArraySize == size)
		return;

	MonoArray *pOldArray = (MonoArray *)m_pObject;

	m_pObject = (MonoObject *)mono_array_new(mono_domain_get(), m_pElementClass, size);

	for(int i = 0; i < size; i++)
	{
		if(i > oldArraySize)
			mono_array_set((MonoArray *)m_pObject, MonoObject *, i, mono_array_get(pOldArray, MonoObject *, i));
	}
}

void CScriptArray::Remove(int index)
{
	int size = GetSize();

	CRY_ASSERT(index < size);

	mono_array_set((MonoArray *)m_pObject, void *, index, nullptr);

	if(index == size - 1)
		m_lastIndex--;
}

IMonoObject *CScriptArray::GetItem(int index)
{ 
	CRY_ASSERT(index <= GetSize());

	if(mono::object monoObj = (mono::object)mono_array_get((MonoArray *)m_pObject, MonoObject *, index))
		return *monoObj;

	return nullptr;
}

void CScriptArray::Insert(mono::object object, int index)
{
	if(index == -1)
	{
		index = m_lastIndex;
		m_lastIndex++;
	}

	CRY_ASSERT(index < GetSize());

	mono_array_set((MonoArray *)m_pObject, void *, index, object);
}

void CScriptArray::InsertNativePointer(void *ptr, int index)
{
	Insert((mono::object)mono_value_box(mono_domain_get(), mono_get_intptr_class(), ptr), index);
}

void CScriptArray::InsertObject(IMonoObject *pObject, int index)
{
	Insert(pObject != nullptr ? pObject->GetManagedObject() : nullptr, index);
}

void CScriptArray::InsertAny(MonoAnyValue value, int index)
{ 
	if(value.type==eMonoAnyType_String)
		Insert((mono::object)ToMonoString(value.str), index);
	else
		Insert(g_pScriptSystem->GetConverter()->BoxAnyValue(value), index);
}

IMonoClass *CScriptArray::GetClass(MonoClass *pClass)
{
	if(CScriptDomain *pDomain = g_pScriptSystem->TryGetDomain(mono_object_get_domain(m_pObject)))
	{
		MonoClass *pMonoClass = GetMonoClass();

		if(CScriptAssembly *pAssembly = pDomain->TryGetAssembly(mono_class_get_image(pMonoClass)))
			return pAssembly->TryGetClass(pMonoClass);
	}

	return nullptr;
}