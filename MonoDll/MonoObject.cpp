#include "StdAfx.h"
#include "MonoObject.h"

#include "MonoClass.h"
#include "MonoAssembly.h"

#include "MonoCVars.h"
#include "MonoScriptSystem.h"

#include <mono/metadata/debug-helpers.h>

CScriptObject::CScriptObject(MonoObject *pObject)
	: m_pClass(NULL)
{
	CRY_ASSERT(pObject);

	m_pObject = pObject;

	// We need this to allow the GC to collect the class object later on.
	m_objectHandle = mono_gchandle_new(m_pObject, false);

	if(MonoProperty *pScriptIdProperty = static_cast<CScriptClass *>(GetClass())->GetMonoProperty("ScriptId"))
		m_scriptId = (int)mono_object_unbox(mono_property_get_value(pScriptIdProperty, m_pObject, nullptr, nullptr));
	else
		m_scriptId = -1;

	gEnv->pMonoScriptSystem->RegisterListener(this);
}

CScriptObject::CScriptObject(MonoObject *object, IMonoArray *pConstructorParams)
	: m_pObject(object)
	, m_pClass(NULL)
	, m_scriptId(-1)
{
	CRY_ASSERT(m_pObject);

	if(pConstructorParams)
		CallMethod(".ctor", pConstructorParams);
	else
		mono_runtime_object_init(m_pObject);

	// We need this to allow the GC to collect the class object later on.
	m_objectHandle = mono_gchandle_new(m_pObject, false);

	gEnv->pMonoScriptSystem->RegisterListener(this);
}

CScriptObject::~CScriptObject()
{
	gEnv->pMonoScriptSystem->UnregisterListener(this);

	 if(m_objectHandle != -1)
		 mono_gchandle_free(m_objectHandle);
	 
	 m_pObject = 0;
}

void CScriptObject::OnPostScriptReload(bool initialLoad)
{
	if(!initialLoad && m_scriptId != -1)
	{
		IMonoArray *pParams = CreateMonoArray(2);
		pParams->Insert(m_scriptId);
		pParams->Insert(eScriptFlag_Any);

		if(IMonoObject *pScriptInstance = gEnv->pMonoScriptSystem->GetScriptManager()->CallMethod("GetScriptInstanceById", pParams, true))
		{
			m_pObject = (MonoObject *)pScriptInstance->GetManagedObject();

			if(m_objectHandle != -1)
				m_objectHandle = mono_gchandle_new(m_pObject, false);
		}
		else
			m_pObject = NULL;

		SAFE_RELEASE(pParams);
	}
}

MonoClass *CScriptObject::GetMonoClass() 
{
	MonoClass *pClass = mono_object_get_class(m_pObject);
	CRY_ASSERT(pClass);

	return pClass;
}

IMonoClass *CScriptObject::GetClass()
{
	if(!m_pClass)
		m_pClass = CScriptAssembly::TryGetClassFromRegistry(GetMonoClass());

	CRY_ASSERT(m_pClass);

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
	MonoMethod *pMethod = static_cast<CScriptClass *>(GetClass())->GetMonoMethod(methodName, pParams);
	CRY_ASSERT(pMethod);

	MonoObject *pException = nullptr;
	MonoObject *pResult = mono_runtime_invoke_array(pMethod, bStatic ? nullptr : m_pObject, pParams ? (MonoArray *)pParams->GetManagedObject() : nullptr, &pException);

	if(pException)
		HandleException(pException);
	else if(pResult)
		return *(mono::object)(pResult);

	return nullptr;
}

IMonoObject *CScriptObject::GetProperty(const char *propertyName, bool bStatic)
{
	MonoProperty *pProperty = static_cast<CScriptClass *>(GetClass())->GetMonoProperty(propertyName);
	CRY_ASSERT(pProperty);

	MonoObject *pException = nullptr;

	MonoObject *propertyValue = mono_property_get_value(pProperty, bStatic ? nullptr : m_pObject, nullptr, &pException);

	if(pException)
		HandleException(pException);
	else if(propertyValue)
		return *(mono::object)propertyValue;

	return nullptr;
}

void CScriptObject::SetProperty(const char *propertyName, IMonoObject *pNewValue, bool bStatic)
{
	MonoProperty *pProperty = static_cast<CScriptClass *>(GetClass())->GetMonoProperty(propertyName);
	CRY_ASSERT(pProperty);

	void *args[1];
	args[0] = pNewValue->GetManagedObject();

	return mono_property_set_value(pProperty, bStatic ? nullptr : m_pObject, args, nullptr);
}

IMonoObject *CScriptObject::GetField(const char *fieldName, bool bStatic)
{
	MonoClassField *pField = static_cast<CScriptClass *>(GetClass())->GetMonoField(fieldName);
	CRY_ASSERT(pField);

	MonoObject *fieldValue = mono_field_get_value_object(mono_domain_get(), pField, bStatic ? nullptr : (MonoObject *)m_pObject);

	if(fieldValue)
		return *(mono::object)fieldValue;

	return nullptr;
}

void CScriptObject::SetField(const char *fieldName, IMonoObject *pNewValue, bool bStatic)
{
	IMonoClass *pClass = GetClass();

	MonoClassField *pField = static_cast<CScriptClass *>(pClass)->GetMonoField(fieldName);
	CRY_ASSERT(pField);

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
		SAFE_RELEASE(args);
	}
	else
	{
		auto method = mono_method_desc_search_in_class(mono_method_desc_new("::ToString()", false), mono_get_exception_class());
		auto stacktrace = (MonoString*)mono_runtime_invoke(method, pException, nullptr, nullptr);
		MonoWarning(ToCryString((mono::string)stacktrace));
	}
}