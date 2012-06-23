#include "StdAfx.h"
#include "MonoConverter.h"

#include <MonoArray.h>
#include <MonoAssembly.h>
#include "MonoClass.h"

#include <MonoAnyValue.h>

IMonoArray *CConverter::CreateArray(int numArgs, IMonoClass *pElementClass)
{
	return new CScriptArray(numArgs, pElementClass); 
}

IMonoArray *CConverter::ToArray(mono::object arr)
{
	CRY_ASSERT(arr);

	return new CScriptArray(arr);
}

IMonoObject *CConverter::ToObject(mono::object obj)
{
	CRY_ASSERT(obj);

	return new CScriptObject((MonoObject *)obj);
}

IMonoObject *CConverter::CreateObject(MonoAnyValue &any)
{
	switch(any.type)
	{
	case eMonoAnyType_Boolean:
		return *(mono::object)mono_value_box(mono_domain_get(), mono_get_boolean_class(), &any.b);
	case eMonoAnyType_Integer:
		return *(mono::object)mono_value_box(mono_domain_get(), mono_get_int32_class(), &any.i);
	case eMonoAnyType_UnsignedInteger:
		return *(mono::object)mono_value_box(mono_domain_get(), mono_get_uint32_class(), &any.u);
	case eMonoAnyType_Short:
		return *(mono::object)mono_value_box(mono_domain_get(), mono_get_int16_class(), &any.i);
	case eMonoAnyType_UnsignedShort:
		return *(mono::object)mono_value_box(mono_domain_get(), mono_get_uint16_class(), &any.u);
	case eMonoAnyType_Float:
		return *(mono::object)mono_value_box(mono_domain_get(), mono_get_single_class(), &any.f);
	case eMonoAnyType_String:
		MonoWarning("IMonoConverter::CreateObject does not support strings");
	case eMonoAnyType_Vec3:
		{
			IMonoClass *pVec3Class = gEnv->pMonoScriptSystem->GetCryBraryAssembly()->GetClass("Vec3");
			return pVec3Class->BoxObject(&any.vec3);
		}
		break;
	}

	return NULL;
}