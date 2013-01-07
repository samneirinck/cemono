#include "stdafx.h"
#include "DynMonoArray.h"

#include "MonoScriptSystem.h"

CDynScriptArray::CDynScriptArray(IMonoClass *pContainingType, int size)
{
	CRY_ASSERT(size >= 0);

	m_lastIndex = -1;

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
		index = m_lastIndex + 1;

	if(size < index || size == 0)
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

void CDynScriptArray::InsertNativePointer(void *ptr, int index)
{
	Insert((mono::object)mono_value_box(mono_domain_get(), mono_get_intptr_class(), ptr), index);
}

void CDynScriptArray::InsertObject(IMonoObject *pObject, int index)
{
	Insert(pObject != nullptr ? pObject->GetManagedObject() : nullptr, index);
}

void CDynScriptArray::InsertAny(MonoAnyValue value, int index)
{ 
	if(value.type==eMonoAnyType_String)
		Insert((mono::object)ToMonoString(value.str), index);
	else
		Insert(g_pScriptSystem->GetConverter()->BoxAnyValue(value), index);
}