#include "StdAfx.h"
#include "MonoConverter.h"

#include <MonoArray.h>
#include <MonoAssembly.h>

#include <MonoAnyValue.h>

CConverter::CConverter()
{
}

CConverter::~CConverter()
{
}

void CConverter::Reset()
{
	m_preStoredTypes.clear();

	IMonoAssembly *pCryBraryAssembly = gEnv->pMonoScriptSystem->GetCryBraryAssembly();

	m_preStoredTypes.insert(TPreStoredTypes::value_type(eCMT_Vec3, pCryBraryAssembly->GetCustomClass("Vec3")));
	m_preStoredTypes.insert(TPreStoredTypes::value_type(eCMT_EntityId, pCryBraryAssembly->GetCustomClass("EntityId")));
	m_preStoredTypes.insert(TPreStoredTypes::value_type(eCMT_HitInfo, pCryBraryAssembly->GetCustomClass("HitInfo")));
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
	for each(auto storedType in m_preStoredTypes)
	{
		if(storedType.first==commonType)
			return ToManagedType(storedType.second, object);
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
	//if(mono_object_get_class((MonoObject *)obj) == mono_get_array_class())
		//return new CScriptArray((mono::array)obj);

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
			return CreateMonoObject<int>((int)any.i);
		}
		break;
	case MONOTYPE_SHORT:
		{
			return CreateMonoObject<short>((short)any.i);
		}
	case MONOTYPE_USHORT:
		{
			return CreateMonoObject<unsigned short>((unsigned short)any.u);
		}
		break;
	case MONOTYPE_UINT:
		{
			return ToManagedType(eCMT_EntityId, new mono::entityId((EntityId)any.u));
		}
		break;
	case MONOTYPE_FLOAT:
		{
			return CreateMonoObject<float>(any.f);
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

IMonoAssembly *CConverter::LoadAssembly(const char *assemblyPath)
{
	return new CScriptAssembly(assemblyPath);
}