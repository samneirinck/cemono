#pragma once

#include <mono/jit/jit.h>
#include "CemonoString.h"

class CCemonoObject
{
public:
	CCemonoObject(MonoObject* pObject);
	virtual ~CCemonoObject();

	MonoClass* GetClass();

	template <class T> T GetPropertyValue(string propertyName)
	{
		return (T)(mono_property_get_value(mono_class_get_property_from_name(GetClass(), propertyName), m_pObject, NULL,NULL));
	}
	template <> const char* GetPropertyValue(string propertyName)
	{
		return CCemonoString::ToString(GetPropertyValue<MonoString*>(propertyName));
	}

	template <class T> T GetPropertyValueAndUnbox(string propertyName)
	{
		return *(reinterpret_cast<T*>(mono_object_unbox(mono_property_get_value(mono_class_get_property_from_name(GetClass(), propertyName), m_pObject, NULL,NULL))));
	}


	operator MonoObject*() {
		return m_pObject;
	}
protected:

	MonoObject* m_pObject;
};

