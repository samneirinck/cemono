#include "StdAfx.h"
#include "MonoObject.h"

#include "MonoClass.h"
#include "MonoCVars.h"

#include <IMonoAssembly.h>

#include <mono/metadata/debug-helpers.h>

CScriptObject::CScriptObject(MonoObject *pObject)
{
	m_pObject = pObject;

	// We need this to allow the GC to collect the class object later on.
	m_objectHandle = mono_gchandle_new(m_pObject, false);
}

CScriptObject::CScriptObject(MonoObject *object, IMonoArray *pConstructorParams)
	: m_pObject(object)
{
	if(pConstructorParams)
		CallMethod(".ctor(string)", pConstructorParams);
	else
		mono_runtime_object_init(m_pObject);

	// We need this to allow the GC to collect the class object later on.
	m_objectHandle = mono_gchandle_new(m_pObject, false);
}

IMonoClass *CScriptObject::GetClass()
{
	return new CScriptClass(GetMonoClass());
}

void CScriptObject::CheckType()
{
	MonoClass *pClass = GetMonoClass();
	if(pClass==mono_get_boolean_class())
		m_type = eMonoAnyType_Boolean;
	else if(pClass==mono_get_int32_class())
		m_type = eMonoAnyType_Integer;
	else if(pClass==mono_get_uint32_class())
		m_type = eMonoAnyType_UnsignedInteger;
	else if(pClass==mono_get_int16_class())
		m_type = eMonoAnyType_Short;
	else if(pClass==mono_get_uint16_class())
		m_type = eMonoAnyType_UnsignedShort;
	else if(pClass==mono_get_single_class())
		m_type = eMonoAnyType_Float;
	else if(pClass==mono_get_string_class())
		m_type = eMonoAnyType_String;
	else if(!strcmp(GetClass()->GetName(), "Vec3"))
		m_type = eMonoAnyType_Vec3;
	else
		m_type = eMonoAnyType_Unknown;
}

IMonoObject *CScriptObject::CallMethod(const char *methodName, IMonoArray *pParams, bool bStatic)
{
	IMonoClass *pClass = GetClass();

	if(MonoMethod *pMethod = static_cast<CScriptClass *>(pClass)->GetMonoMethod(methodName, pParams))
	{
		MonoObject *pException = NULL;

		MonoObject *pResult = mono_runtime_invoke_array(pMethod, bStatic ? NULL : m_pObject, pParams ? (MonoArray *)pParams->GetManagedObject() : NULL, &pException);

		if(pException)
			HandleException(pException);
		else if(pResult)
			return *(mono::object)(pResult);
	}


	return NULL;
}

IMonoObject *CScriptObject::GetProperty(const char *propertyName, bool bStatic)
{
	IMonoClass *pClass = GetClass();

	if(MonoProperty *pProperty = static_cast<CScriptClass *>(pClass)->GetMonoProperty(propertyName))
	{
		MonoObject *pException = NULL;

		MonoObject *propertyValue = mono_property_get_value(pProperty, bStatic ? NULL : m_pObject, NULL, &pException);

		if(pException)
			HandleException(pException);
		else if(propertyValue)
			return *(mono::object)propertyValue;
	}

	return NULL;
}

void CScriptObject::SetProperty(const char *propertyName, IMonoObject *pNewValue, bool bStatic)
{
	IMonoClass *pClass = GetClass();

	if(MonoProperty *pProperty = static_cast<CScriptClass *>(pClass)->GetMonoProperty(propertyName))
	{
		void *args[1];
		args[0] = pNewValue->GetManagedObject();

		return mono_property_set_value(pProperty, bStatic ? NULL : m_pObject, args, NULL);
	}
}

IMonoObject *CScriptObject::GetField(const char *fieldName, bool bStatic)
{
	IMonoClass *pClass = GetClass();

	if(MonoClassField *pField = static_cast<CScriptClass *>(pClass)->GetMonoField(fieldName))
	{
		MonoObject *fieldValue = mono_field_get_value_object(mono_domain_get(), pField, bStatic ? NULL : (MonoObject *)m_pObject);

		if(fieldValue)
			return *(mono::object)fieldValue;
	}

	return NULL;
}

void CScriptObject::SetField(const char *fieldName, IMonoObject *pNewValue, bool bStatic)
{
	IMonoClass *pClass = GetClass();

	if(MonoClassField *pField = static_cast<CScriptClass *>(pClass)->GetMonoField(fieldName))
		return mono_field_set_value(bStatic ? NULL : (MonoObject *)m_pObject, pField, pNewValue->GetManagedObject());
}

void CScriptObject::HandleException(MonoObject *pException)
{
	MonoMethod *pExceptionMethod = mono_method_desc_search_in_class(mono_method_desc_new("::ToString()", false),mono_get_exception_class());
	MonoString *exceptionString = (MonoString *)mono_runtime_invoke(pExceptionMethod, pException, NULL, NULL);

	if(g_pMonoCVars->mono_exceptionsTriggerMessageBoxes)
		CryMessageBox(ToCryString((mono::string)exceptionString), "CryMono exception was raised", 0x00000000L);

	if(g_pMonoCVars->mono_exceptionsTriggerFatalErrors)
		CryFatalError(ToCryString((mono::string)exceptionString));
	else
		MonoWarning(ToCryString((mono::string)exceptionString));
}