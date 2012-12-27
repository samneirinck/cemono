#include "StdAfx.h"
#include "MonoObject.h"

#include "MonoDomain.h"
#include "MonoAssembly.h"
#include "MonoClass.h"

#include "MonoCVars.h"
#include "MonoScriptSystem.h"

#include <mono/metadata/debug-helpers.h>

CScriptObject::CScriptObject(MonoObject *pObject, bool allowGC)
	: m_pObject(NULL)
{
	SetManagedObject(pObject, allowGC);
}

CScriptObject::CScriptObject(MonoObject *object, IMonoArray *pConstructorParams)
	: m_pObject(NULL)
{
	SetManagedObject(object, true);

	if(pConstructorParams)
		GetClass()->InvokeArray(this, ".ctor", pConstructorParams);
	else
		mono_runtime_object_init(m_pObject);
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
	if(m_pClass == NULL)
	{
		if(CScriptDomain *pDomain = g_pScriptSystem->TryGetDomain(mono_object_get_domain(m_pObject)))
		{
			MonoClass *pMonoClass = GetMonoClass();

			if(CScriptAssembly *pAssembly = pDomain->TryGetAssembly(mono_class_get_image(pMonoClass)))
				m_pClass = pAssembly->TryGetClass(pMonoClass);
		}
	}

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
	else if(pMonoClass == mono_get_array_class())
		return eMonoAnyType_Array;
	else
	{
		const char *className = mono_class_get_name(pMonoClass);
		if(!strcmp(className, "EntityId"))
			return eMonoAnyType_EntityId;
		else if(!strcmp(className, "Vec3"))
			return eMonoAnyType_Vec3;
	}

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
	case eMonoAnyType_EntityId:
		{
			MonoAnyValue value = Unbox<EntityId>();
			value.type = eMonoAnyType_EntityId;

			return value;
		}
	case eMonoAnyType_Short:
		return Unbox<short>();
	case eMonoAnyType_UnsignedShort:
		return Unbox<unsigned short>();
	case eMonoAnyType_Float:
		return Unbox<float>();
	case eMonoAnyType_Vec3:
		return Unbox<Vec3>();
	case eMonoAnyType_String:
		return ToCryString((mono::string)GetManagedObject());
	}

	return MonoAnyValue();
}

const char *CScriptObject::ToString()
{
	MonoObject *pException = nullptr;

	MonoMethod *method = mono_method_desc_search_in_class(mono_method_desc_new("::ToString()", false), GetMonoClass());
	MonoObject *pResult = mono_runtime_invoke(method, m_pObject, nullptr, &pException);

	if(pException)
		HandleException(pException);
	else
		return ToCryString((mono::string)pResult);

	return nullptr;
}

void CScriptObject::HandleException(MonoObject *pException)
{
	// Fatal errors override disabling the message box option
	bool isFatal = g_pMonoCVars->mono_exceptionsTriggerFatalErrors != 0;

	IMonoAssembly *pCryBraryAssembly = g_pScriptSystem->GetCryBraryAssembly();

	if((g_pMonoCVars->mono_exceptionsTriggerMessageBoxes || isFatal) && pCryBraryAssembly)
	{
		auto args = CreateMonoArray(2);
		args->InsertObject(*(mono::object)pException);
		args->Insert(isFatal);

		IMonoClass *pDebugClass = pCryBraryAssembly->GetClass("Debug");
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

void CScriptObject::SetManagedObject(MonoObject *newObject, bool allowGC)
{
	CRY_ASSERT(newObject);

	m_pClass = NULL; // Class pointer is most definitely invalid now, force recollection on next GetClass call.
	m_pObject = newObject;

	// We need this to allow the GC to collect the class object later on.
	if(allowGC)
		m_objectHandle = mono_gchandle_new(m_pObject, false);
	else
		m_objectHandle = -1;
}