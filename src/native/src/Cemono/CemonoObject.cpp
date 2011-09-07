#include "StdAfx.h"
#include "CemonoObject.h"


CCemonoObject::CCemonoObject(MonoObject* pObject)
{
	m_pObject = pObject;
}


CCemonoObject::~CCemonoObject()
{
}

MonoClass* CCemonoObject::GetClass()
{
	return mono_object_get_class(m_pObject);
}
