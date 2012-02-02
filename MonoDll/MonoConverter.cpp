#include "StdAfx.h"
#include "MonoConverter.h"

#include <MonoArray.h>
#include <IMonoAssembly.h>

#include <MonoAnyValue.h>

CMonoConverter::CMonoConverter()
{
}


CMonoConverter::~CMonoConverter()
{
}

IMonoArray *CMonoConverter::CreateArray(int numArgs)
{
	return new CMonoArray(numArgs); 
}

IMonoArray *CMonoConverter::ToArray(mono::array arr)
{
	return new CMonoArray(arr);
}

IMonoObject *CMonoConverter::ToManagedType(IMonoClass *pTo, void *object)
{
	if(pTo)
		return *(mono::object)mono_value_box(mono_domain_get(), static_cast<CMonoClass *>(pTo)->GetMonoClass(), object);

	return NULL;
}

IMonoObject *CMonoConverter::ToObject(mono::object obj)
{
	return new CMonoObject((MonoObject *)obj);
}

IMonoClass *CMonoConverter::ToClass(IMonoObject *pObject)
{
	MonoClass *pClass = (MonoClass *)pObject->GetMonoObject();

	if(pClass && mono_class_get_name(pClass))
		return new CMonoClass(pClass); // We received a class (non-instantiated / constructed)
	else
	{
		MonoObject *pMonoObject = (MonoObject *)pObject->GetMonoObject();

		if(pMonoObject)
		{
			pClass = mono_object_get_class(pMonoObject);
			// If this passes, we have a instantiated class.
			if(pClass && mono_class_get_name(pClass))
				return new CMonoClass(pClass, pMonoObject);
		}
	}

	return NULL;
}

IMonoObject *CMonoConverter::CreateObject(MonoAnyValue &any)
{
	IMonoObject *pObject = NULL;

	switch(any.type)
	{
	case MONOTYPE_BOOL:
		{
			return CreateMonoObject<bool>(any.b);
		}
		break;
	case MONOTYPE_INT:
		{
			return CreateMonoObject<int>((int)any.number);
		}
		break;
	case MONOTYPE_USHORT:
		{
			return CreateMonoObject<unsigned short>((unsigned short)any.number);
		}
		break;
	case MONOTYPE_UINT:
		{
			return CreateMonoObject<unsigned int>((unsigned int)any.number);
		}
		break;
	case MONOTYPE_FLOAT:
		{
			return CreateMonoObject<float>(any.number);
		}
		break;
	case MONOTYPE_VEC3:
	case MONOTYPE_ANG3:
		{
			return gEnv->pMonoScriptSystem->GetConverter()->ToManagedType(gEnv->pMonoScriptSystem->GetCryBraryAssembly()->GetCustomClass("Vec3"), Vec3(any.vec3.x, any.vec3.y, any.vec3.z));
		}
		break;
	}

	return NULL;
}