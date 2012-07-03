#include "StdAfx.h"
#include "MonoObject.h"

#include "MonoClass.h"
#include "MonoAssembly.h"

#include "MonoCVars.h"

#include <mono/metadata/debug-helpers.h>

CScriptObject::CScriptObject(MonoObject *pObject)
	: m_pClass(NULL)
{
	CRY_ASSERT(pObject);

	m_pObject = pObject;

	// We need this to allow the GC to collect the class object later on.
	m_objectHandle = mono_gchandle_new(m_pObject, false);
}

CScriptObject::CScriptObject(MonoObject *object, IMonoArray *pConstructorParams)
	: m_pObject(object)
	, m_pClass(NULL)
{
	CRY_ASSERT(m_pObject);

	if(pConstructorParams)
		CallMethod(".ctor", pConstructorParams);
	else
		mono_runtime_object_init(m_pObject);

	// We need this to allow the GC to collect the class object later on.
	m_objectHandle = mono_gchandle_new(m_pObject, false);
}

void CScriptObject::SetObject(mono::object object) 
{
	m_pObject = (MonoObject *)object; 

	if(m_objectHandle != -1) // Don't set the gc handle if we've previously refrained from it
		m_objectHandle = mono_gchandle_new(m_pObject, false);
}

IMonoClass *CScriptObject::GetClass()
{
	if(!m_pClass)
		m_pClass = CScriptAssembly::TryGetClassFromRegistry(GetMonoClass());

	return m_pClass;
}

EMonoAnyType CScriptObject::GetType()
{
	MonoClass *pMonoClass = GetMonoClass();

	if(pMonoClass==mono_get_boolean_class())
		return eMonoAnyType_Boolean;
	else if(pMonoClass==mono_get_int32_class())
		return eMonoAnyType_Integer;
	else if(pMonoClass==mono_get_uint32_class())
		return eMonoAnyType_UnsignedInteger;
	else if(pMonoClass==mono_get_int16_class())
		return eMonoAnyType_Short;
	else if(pMonoClass==mono_get_uint16_class())
		return eMonoAnyType_UnsignedShort;
	else if(pMonoClass==mono_get_single_class())
		return eMonoAnyType_Float;
	else if(pMonoClass==mono_get_string_class())
		return eMonoAnyType_String;
	else if(pMonoClass==mono_get_intptr_class())
		return eMonoAnyType_IntPtr;
	//else if(!strcmp(className, "Vec3"))
		//return eMonoAnyType_Vec3;

	return eMonoAnyType_Unknown;
}

IMonoObject *CScriptObject::CallMethod(const char *methodName, IMonoArray *pParams, bool bStatic)
{
	if(!m_pClass)
		m_pClass = CScriptAssembly::TryGetClassFromRegistry(GetMonoClass());

	if(MonoMethod *pMethod = m_pClass->GetMonoMethod(methodName, pParams))
	{
		MonoObject *pException = nullptr;
		MonoObject *pResult = mono_runtime_invoke_array(pMethod, bStatic ? nullptr : m_pObject, pParams ? (MonoArray *)pParams->GetManagedObject() : nullptr, &pException);

		if(pException)
			HandleException(pException);
		else if(pResult)
			return *(mono::object)(pResult);
	}

	return nullptr;
}

IMonoObject *CScriptObject::GetProperty(const char *propertyName, bool bStatic)
{
	IMonoClass *pClass = GetClass();

	if(MonoProperty *pProperty = static_cast<CScriptClass *>(pClass)->GetMonoProperty(propertyName))
	{
		MonoObject *pException = nullptr;

		MonoObject *propertyValue = mono_property_get_value(pProperty, bStatic ? nullptr : m_pObject, nullptr, &pException);

		if(pException)
			HandleException(pException);
		else if(propertyValue)
			return *(mono::object)propertyValue;
	}

	return nullptr;
}

void CScriptObject::SetProperty(const char *propertyName, IMonoObject *pNewValue, bool bStatic)
{
	IMonoClass *pClass = GetClass();

	if(MonoProperty *pProperty = static_cast<CScriptClass *>(pClass)->GetMonoProperty(propertyName))
	{
		void *args[1];
		args[0] = pNewValue->GetManagedObject();

		return mono_property_set_value(pProperty, bStatic ? nullptr : m_pObject, args, nullptr);
	}
}

IMonoObject *CScriptObject::GetField(const char *fieldName, bool bStatic)
{
	IMonoClass *pClass = GetClass();

	if(MonoClassField *pField = static_cast<CScriptClass *>(pClass)->GetMonoField(fieldName))
	{
		MonoObject *fieldValue = mono_field_get_value_object(mono_domain_get(), pField, bStatic ? nullptr : (MonoObject *)m_pObject);

		if(fieldValue)
			return *(mono::object)fieldValue;
	}

	return nullptr;
}

void CScriptObject::SetField(const char *fieldName, IMonoObject *pNewValue, bool bStatic)
{
	IMonoClass *pClass = GetClass();

	if(MonoClassField *pField = static_cast<CScriptClass *>(pClass)->GetMonoField(fieldName))
		return mono_field_set_value(bStatic ? nullptr : (MonoObject *)m_pObject, pField, pNewValue->GetManagedObject());
}

void CScriptObject::HandleException(MonoObject *pException)
{
	// Fatal errors override disabling the message box option
	bool isFatal = g_pMonoCVars->mono_exceptionsTriggerFatalErrors != 0;

	if(g_pMonoCVars->mono_exceptionsTriggerMessageBoxes || isFatal)
	{
		auto args = CreateMonoArray(2);
		args->InsertObject(*(mono::object)pException);
		args->Insert(isFatal);

		auto form = gEnv->pMonoScriptSystem->GetCryBraryAssembly()->GetClass("ExceptionMessage")->CreateInstance(args);
		form->CallMethod("ShowDialog");
	}
	else
	{
		auto method = mono_method_desc_search_in_class(mono_method_desc_new("::ToString()", false), mono_get_exception_class());
		auto stacktrace = (MonoString*)mono_runtime_invoke(method, pException, nullptr, nullptr);
		MonoWarning(ToCryString((mono::string)stacktrace));
	}
}