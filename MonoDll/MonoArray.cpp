#include "StdAfx.h"
#include "MonoArray.h"

#include "MonoDomain.h"
#include "MonoObject.h"

#include "MonoScriptSystem.h"

#include <IMonoClass.h>

MonoClass *CScriptArray::m_pDefaultElementClass = NULL;

CScriptArray::CScriptArray(mono::object managedArray, bool allowGC)
{
	CRY_ASSERT(managedArray);

	SetManagedObject((MonoObject *)managedArray, allowGC);
	m_pElementClass = mono_class_get_element_class(GetMonoClass());

	m_lastIndex = GetSize() - 1;
}

CScriptArray::CScriptArray(MonoDomain *pDomain, int size, IMonoClass *pContainingType, bool allowGC)
	: m_lastIndex(-1)
{
	CRY_ASSERT(size >= 0);
	CRY_ASSERT(pDomain);

	m_pElementClass = (pContainingType ? (MonoClass *)(pContainingType->GetManagedObject()) : m_pDefaultElementClass);
	CRY_ASSERT(m_pElementClass);

	SetManagedObject((MonoObject *)mono_array_new(pDomain, m_pElementClass, size), allowGC);
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

	CScriptDomain *pDomain = static_cast<CScriptDomain *>(GetClass()->GetAssembly()->GetDomain());

	m_pObject = (MonoObject *)mono_array_new(pDomain->GetMonoDomain(), m_pElementClass, size);

	for(int i = 0; i < oldArraySize; i++)
	{
		if(i < size)
		{
			mono_array_set((MonoArray *)m_pObject, MonoObject *, i, mono_array_get(pOldArray, MonoObject *, i));
			m_lastIndex = i;
		}
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

mono::object CScriptArray::GetItem(int index)
{ 
	CRY_ASSERT(index < GetSize());

	return (mono::object)mono_array_get((MonoArray *)m_pObject, MonoObject *, index);
}

void CScriptArray::InsertMonoObject(mono::object object, int index)
{
	if(index == -1)
	{
		m_lastIndex++;
		index = m_lastIndex;
	}

	CRY_ASSERT(index < GetSize());

	mono_array_set((MonoArray *)m_pObject, void *, index, object);
}

void CScriptArray::InsertNativePointer(void *ptr, int index)
{
	CScriptDomain *pDomain = static_cast<CScriptDomain *>(GetClass()->GetAssembly()->GetDomain());

	InsertMonoObject((mono::object)mono_value_box(pDomain->GetMonoDomain(), mono_get_intptr_class(), (long *)&ptr), index);
}

void CScriptArray::InsertAny(MonoAnyValue value, int index)
{ 
	IMonoDomain *pDomain = GetClass()->GetAssembly()->GetDomain();

	InsertMonoObject(pDomain->BoxAnyValue(value), index);
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