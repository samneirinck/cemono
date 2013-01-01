#include "StdAfx.h"
#include "MonoConverter.h"

#include "MonoCVars.h"

#include "MonoScriptSystem.h"
#include "MonoArray.h"
#include "DynMonoArray.h"
#include "MonoObject.h"

#include <IMonoAssembly.h>

IMonoArray *CConverter::CreateArray(int numArgs, IMonoClass *pElementClass)
{
	return new CScriptArray(numArgs, pElementClass); 
}

IMonoArray *CConverter::CreateDynamicArray(IMonoClass *pElementClass, int size)
{
	return new CDynScriptArray(pElementClass, size);
}

IMonoArray *CConverter::ToArray(mono::object arr)
{
	CRY_ASSERT(arr);

	return new CScriptArray(arr);
}

IMonoObject *CConverter::ToObject(mono::object obj, bool allowGC)
{
	CRY_ASSERT(obj);

	return new CScriptObject((MonoObject *)obj, allowGC);
}

mono::object CConverter::BoxAnyValue(MonoAnyValue &any)
{
	switch(any.type)
	{
	case eMonoAnyType_Boolean:
		return (mono::object)mono_value_box(mono_domain_get(), mono_get_boolean_class(), &any.b);
	case eMonoAnyType_Integer:
		return (mono::object)mono_value_box(mono_domain_get(), mono_get_int32_class(), &any.i);
	case eMonoAnyType_UnsignedInteger:
		return (mono::object)mono_value_box(mono_domain_get(), mono_get_uint32_class(), &any.u);
	case eMonoAnyType_EntityId:
		{
			IMonoClass *pEntityIdClass = g_pScriptSystem->GetCryBraryAssembly()->GetClass("EntityId");
			return pEntityIdClass->BoxObject(&mono::entityId(any.u))->GetManagedObject();
		}
	case eMonoAnyType_Short:
		return (mono::object)mono_value_box(mono_domain_get(), mono_get_int16_class(), &any.i);
	case eMonoAnyType_UnsignedShort:
		return (mono::object)mono_value_box(mono_domain_get(), mono_get_uint16_class(), &any.u);
	case eMonoAnyType_Float:
		return (mono::object)mono_value_box(mono_domain_get(), mono_get_single_class(), &any.f);
	case eMonoAnyType_String:
		MonoWarning("IMonoConverter::BoxAnyValue does not support strings, utilize ToMonoString instead");
	case eMonoAnyType_Vec3:
		{
			IMonoClass *pVec3Class = g_pScriptSystem->GetCryBraryAssembly()->GetClass("Vec3");
			return pVec3Class->BoxObject(&any.vec3)->GetManagedObject();
		}
		break;
	}

	return nullptr;
}