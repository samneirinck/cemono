#include "StdAfx.h"
#include "MonoObject.h"

#include <IMonoAssembly.h>

MonoAnyType CScriptObject::GetType()
{
	MonoAnyType type = MONOTYPE_NULL;

	// :eek:
	MonoClass *pClass = GetMonoClass();
	if(pClass==mono_get_int32_class())
		type = MONOTYPE_INT;
	else if(pClass==mono_get_single_class())
		type = MONOTYPE_FLOAT;
	else if(pClass==mono_get_boolean_class())
		type = MONOTYPE_BOOL;
	else if(pClass==mono_get_uint16_class())
		type = MONOTYPE_USHORT;
	else if(pClass==mono_get_uint32_class())
		type = MONOTYPE_UINT;
	else if(pClass==static_cast<CScriptClass *>(gEnv->pMonoScriptSystem->GetCryBraryAssembly()->GetCustomClass("Vec3"))->GetMonoClass())
		type = MONOTYPE_VEC3;
	else if(pClass==mono_get_string_class())
		type = MONOTYPE_STRING;

	return type;
}

MonoAnyValue CScriptObject::GetAnyValue()
{
	switch(GetType())
	{
	case MONOTYPE_BOOL:
		return MonoAnyValue(Unbox<bool>());
	case MONOTYPE_INT:
		return MonoAnyValue(Unbox<int>());
	case MONOTYPE_USHORT:
		return MonoAnyValue(Unbox<unsigned short>());
	case MONOTYPE_UINT:
		return MonoAnyValue(Unbox<unsigned int>());
	case MONOTYPE_FLOAT:
		return MonoAnyValue(Unbox<float>());
	case MONOTYPE_VEC3:
		return MonoAnyValue(Unbox<Vec3>());
	case MONOTYPE_STRING:
		return MonoAnyValue(ToCryString((mono::string)m_pObject));
	}

	return (MonoAnyValue)0;
}