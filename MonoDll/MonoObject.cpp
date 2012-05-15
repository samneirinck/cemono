#include "StdAfx.h"
#include "MonoObject.h"

#include <IMonoAssembly.h>

CScriptObject::CScriptObject(mono::object object) 
{
	m_pObject = object; 

	m_objectHandle = mono_gchandle_new((MonoObject *)object, false); 
}

EMonoAnyType CScriptObject::GetType()
{
	MonoClass *pClass = GetMonoClass();
	if(pClass==mono_get_boolean_class())
		return eMonoAnyType_Boolean;
	else if(pClass==mono_get_int32_class())
		return eMonoAnyType_Integer;
	else if(pClass==mono_get_uint32_class())
		return eMonoAnyType_UnsignedInteger;
	else if(pClass==mono_get_int16_class())
		return eMonoAnyType_Short;
	else if(pClass==mono_get_uint16_class())
		return eMonoAnyType_UnsignedShort;
	else if(pClass==mono_get_single_class())
		return eMonoAnyType_Float;
	else if(pClass==mono_get_string_class())
		return eMonoAnyType_String;
	else if(pClass==static_cast<CScriptClass *>(gEnv->pMonoScriptSystem->GetConverter()->GetCommonClass(eCMT_Vec3))->GetMonoClass())
		return eMonoAnyType_Vec3;

	return eMonoAnyType_NULL;
}

MonoAnyValue CScriptObject::GetAnyValue()
{
	switch(GetType())
	{
	case eMonoAnyType_Boolean:
		return MonoAnyValue(Unbox<bool>());
	case eMonoAnyType_Integer:
		return MonoAnyValue(Unbox<int>());
	case eMonoAnyType_UnsignedInteger:
		return MonoAnyValue(Unbox<unsigned int>());
	case eMonoAnyType_Short:
		return MonoAnyValue(Unbox<short>());
	case eMonoAnyType_UnsignedShort:
		return MonoAnyValue(Unbox<unsigned short>());
	case eMonoAnyType_Float:
		return MonoAnyValue(Unbox<float>());
	case eMonoAnyType_Vec3:
		return MonoAnyValue(Unbox<Vec3>());
	case eMonoAnyType_String:
		return MonoAnyValue(ToCryString((mono::string)m_pObject));
	}

	return MonoAnyValue();
}