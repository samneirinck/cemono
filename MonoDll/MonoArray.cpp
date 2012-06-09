#include "StdAfx.h"
#include "MonoArray.h"

#include "MonoObject.h"

CScriptArray::CScriptArray(int size)
	: m_curIndex(0)
{
	m_pArray = (mono::array)mono_array_new(mono_domain_get(), mono_get_object_class(), size);
}

CScriptArray::~CScriptArray()
{
	m_curIndex = 0;
	m_pArray = 0;

	mono_gchandle_free(m_arrayHandle); 
}

void CScriptArray::Resize(int size)
{
	MonoArray *pOldArray = (MonoArray *)m_pArray;
	int oldArraySize = mono_array_length(pOldArray);

	m_pArray = (mono::array)mono_array_new(mono_domain_get(), mono_get_object_class(), size);

	for(int i = 0; i < size; i++)
	{
		if(i > oldArraySize)
			mono_array_set((MonoArray *)m_pArray, MonoObject *, i, mono_array_get(pOldArray, MonoObject *, i));
	}
}

IMonoObject *CScriptArray::GetItem(int index)
{ 
	if(index <= GetSize())
	{
		if(mono::object monoObj = (mono::object)mono_array_get((MonoArray *)m_pArray, MonoObject *, index))
			return *monoObj;
	}
	else
		CryLogAlways("[Warning] Index out of range exception: Attempted to access index %i on IMonoArray of size %i", index, GetSize());

	return NULL;
}

IMonoArray *CScriptArray::GetItemArray(int index) 
{
	if(mono::array monoArray = (mono::array)mono_array_get((MonoArray *)m_pArray, MonoArray *, index))
		return new CScriptArray(monoArray);

	return NULL;
}

void CScriptArray::InsertMonoObject(mono::object object, int index)
{
	if((index == -1 && m_curIndex >= GetSize()) || index >= GetSize())
	{
		CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_WARNING, "Attempted to insert too many objects into array of size %i", GetSize());
		return;
	}

	mono_array_set((MonoArray *)m_pArray, MonoObject *, m_curIndex, (MonoObject *)object);

	m_curIndex++;
}

void CScriptArray::InsertMonoString(mono::string string, int index)
{
	if((index == -1 && m_curIndex >= GetSize()) || index >= GetSize())
	{
		CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_WARNING, "Attempted to insert too many objects into array of size %i", GetSize());
		return;
	}

	mono_array_set((MonoArray *)m_pArray, MonoString *, index != -1 ? index : m_curIndex, (MonoString *)string);

	m_curIndex++;
}

void CScriptArray::InsertMonoArray(mono::array arr, int index)
{
	if((index == -1 && m_curIndex >= GetSize()) || index >= GetSize())
	{
		CryWarning(VALIDATOR_MODULE_GAME, VALIDATOR_WARNING, "Attempted to insert too many objects into array of size %i", GetSize());
		return;
	}

	mono_array_set((MonoArray *)m_pArray, MonoArray *, index != -1 ? index : m_curIndex, (MonoArray *)arr);

	m_curIndex++;
}

void CScriptArray::InsertObject(IMonoObject *pObject, int index)
{ 
	InsertMonoObject(pObject->GetMonoObject(), index); 
}

void CScriptArray::InsertAny(MonoAnyValue value, int index)
{ 
	if(value.type==eMonoAnyType_String)
		InsertMonoString(ToMonoString(value.str), index);
	else
		Insert(gEnv->pMonoScriptSystem->GetConverter()->CreateObject(value), index);
}

void CScriptArray::InsertArray(IMonoArray *pArray, int index)
{
	InsertMonoArray(pArray ? *pArray : (mono::array)NULL, index);
}