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

	if(IMonoObject *pScriptId = GetPropertyValue("ScriptId"))
		m_scriptId = pScriptId->Unbox<int>();
	else
		m_scriptId = -1;
}

CScriptObject::CScriptObject(MonoObject *object, IMonoArray *pConstructorParams)
	: m_pObject(object)
	, m_pClass(NULL)
	, m_scriptId(-1)
{
	CRY_ASSERT(m_pObject);

	if(pConstructorParams)
		GetClass()->InvokeArray(this, ".ctor", pConstructorParams);
	else
		mono_runtime_object_init(m_pObject);

	// We need this to allow the GC to collect the class object later on.
	m_objectHandle = mono_gchandle_new(m_pObject, false);
}

CScriptObject::~CScriptObject()
{
	// Decrement ref counter in the class, released if no longer used.
	SAFE_RELEASE(m_pClass);

	 if(m_objectHandle != -1)
		 mono_gchandle_free(m_objectHandle);
	 
	 m_pObject = 0;
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

MonoAnyValue CScriptObject::GetAnyValue()
{
	switch(GetType())
	{
	case eMonoAnyType_Boolean:
		return Unbox<bool>();
	case eMonoAnyType_Integer:
		return Unbox<int>();
	case eMonoAnyType_UnsignedInteger:
		return Unbox<uint>();
	case eMonoAnyType_Short:
		return Unbox<short>();
	case eMonoAnyType_UnsignedShort:
		return Unbox<unsigned short>();
	case eMonoAnyType_Float:
		return Unbox<float>();
	case eMonoAnyType_String:
		return ToCryString((mono::string)GetManagedObject());
	}

	return MonoAnyValue();
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

		IMonoClass *pDebugClass = gEnv->pMonoScriptSystem->GetCryBraryAssembly()->GetClass("Debug");
		pDebugClass->InvokeArray(NULL, "DisplayException", args);
		SAFE_RELEASE(args);
	}
	else
	{
		auto method = mono_method_desc_search_in_class(mono_method_desc_new("::ToString()", false), mono_get_exception_class());
		auto stacktrace = (MonoString*)mono_runtime_invoke(method, pException, nullptr, nullptr);
		MonoWarning(ToCryString((mono::string)stacktrace));
	}
}