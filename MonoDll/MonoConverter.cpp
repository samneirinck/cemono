#include "StdAfx.h"
#include "MonoConverter.h"

#include <MonoArray.h>
#include <IMonoAssembly.h>

#include <MonoAnyValue.h>

CConverter::CConverter()
{
}

CConverter::~CConverter()
{
}

void CConverter::Reset()
{
	m_pVec3Type = gEnv->pMonoScriptSystem->GetCryBraryAssembly()->GetCustomClass("Vec3");
	m_pEntityIdType = gEnv->pMonoScriptSystem->GetCryBraryAssembly()->GetCustomClass("EntityId");
}

IMonoArray *CConverter::CreateArray(int numArgs)
{
	return new CScriptArray(numArgs); 
}

IMonoArray *CConverter::ToArray(mono::array arr)
{
	return new CScriptArray(arr);
}

IMonoObject *CConverter::ToManagedType(ECommonManagedTypes commonType, void *object)
{
	switch(commonType)
	{
	case eCMT_Vec3:
		return ToManagedType(m_pVec3Type, object);
	case eCMT_EntityId:
		return ToManagedType(m_pEntityIdType, object);
	}

	return NULL;
}

IMonoObject *CConverter::ToManagedType(IMonoClass *pTo, void *object)
{
	if(pTo)
		return *(mono::object)mono_value_box(mono_domain_get(), static_cast<CScriptClass *>(pTo)->GetMonoClass(), object);

	return NULL;
}

IMonoObject *CConverter::ToObject(mono::object obj)
{
	return new CScriptObject(obj);
}

IMonoClass *CConverter::ToClass(IMonoObject *pObject)
{
	mono::object pMonoObject = pObject->GetMonoObject();

	MonoClass *pClass = mono_object_get_class((MonoObject *)pMonoObject);
	if(pClass && mono_class_get_name(pClass))
		return new CScriptClass(pClass, pMonoObject);

	return NULL;
}

struct SMonoEntityId
{
	SMonoEntityId(EntityId id) : value(id) {}

	EntityId value;
};

IMonoObject *CConverter::CreateObject(MonoAnyValue &any)
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
	case MONOTYPE_SHORT:
		{
			return CreateMonoObject<short>((short)any.number);
		}
	case MONOTYPE_USHORT:
		{
			return CreateMonoObject<unsigned short>((unsigned short)any.number);
		}
		break;
	case MONOTYPE_UINT:
		{
			return ToManagedType(eCMT_EntityId, new SMonoEntityId(any.number));
		}
		break;
	case MONOTYPE_FLOAT:
		{
			return CreateMonoObject<float>(any.number);
		}
		break;
	case MONOTYPE_VEC3:
		{
			return ToManagedType(eCMT_Vec3, Vec3(any.vec3.x, any.vec3.y, any.vec3.z));
		}
		break;
	}

	return NULL;
}