#include "stdafx.h"
#include "DynMonoArray.h"

#include "MonoScriptSystem.h"
#include "MonoDomain.h"

CDynScriptArray::CDynScriptArray(MonoDomain *pDomain, IMonoClass *pContainingType, int size)
{
	CRY_ASSERT(size >= 0);
	CRY_ASSERT(pDomain);

	m_lastIndex = -1;

	m_pElementClass = (pContainingType ? (MonoClass *)(pContainingType)->GetManagedObject() : m_pDefaultElementClass);
	CRY_ASSERT(m_pElementClass);

	m_pObject = (MonoObject *)mono_array_new(pDomain, m_pElementClass, size);
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
	CScriptDomain *pDomain = static_cast<CScriptDomain *>(GetClass()->GetAssembly()->GetDomain());

	Insert((mono::object)mono_value_box(pDomain->GetMonoDomain(), mono_get_intptr_class(), ptr), index);
}

void CDynScriptArray::InsertObject(IMonoObject *pObject, int index)
{
	Insert(pObject != nullptr ? pObject->GetManagedObject() : nullptr, index);
}

void CDynScriptArray::InsertAny(MonoAnyValue value, int index)
{ 
	IMonoDomain *pDomain = GetClass()->GetAssembly()->GetDomain();

	if(value.type==eMonoAnyType_String)
		Insert((mono::object)pDomain->CreateMonoString(value.str), index);
	else
		Insert(pDomain->BoxAnyValue(value), index);
}