#include "stdafx.h"
#include "DynMonoArray.h"

CDynScriptArray::CDynScriptArray(IMonoClass *pContainingType, int size)
{
	CRY_ASSERT(size > 0);

	m_lastIndex = 0;

	m_pElementClass = (pContainingType ? (MonoClass *)(pContainingType)->GetManagedObject() : m_pDefaultElementClass);
	CRY_ASSERT(m_pElementClass);

	m_pObject = (MonoObject *)mono_array_new(mono_domain_get(), m_pElementClass, size);
	m_objectHandle = mono_gchandle_new(m_pObject, false);

	m_pClass = NULL;
}

void CDynScriptArray::Clear()
{
	SAFE_DELETE(m_pObject);
}

void CDynScriptArray::Insert(mono::object object, int index)
{
	int size = GetSize();

	if(index == -1)
		Resize(size + 1);
	else if(index >= size)
		Resize(index + 1);

	CScriptArray::Insert(object, index);
}

void CDynScriptArray::Remove(int index)
{
	if(index == m_lastIndex)
		Resize(m_lastIndex);
	else
		CScriptArray::Remove(index);
}