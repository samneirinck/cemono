#include "stdafx.h"
#include "CemonoArray.h"

CCemonoArray::CCemonoArray(MonoArray* pArray)
{
	m_pArray = pArray;
}

CCemonoArray::~CCemonoArray()
{

}

int CCemonoArray::GetLength() const
{
	return mono_array_length(m_pArray);
}

CCemonoObject  CCemonoArray::GetItem(int index)
{
	return CCemonoObject(mono_array_get(m_pArray, MonoObject*, index));
}