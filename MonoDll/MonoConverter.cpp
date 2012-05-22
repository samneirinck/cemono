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
	m_preStoredTypes.insert(TPreStoredTypes::value_type(eCMT_PointerWrapper, pCryBraryAssembly->GetCustomClass("PointerWrapper")));
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
	switch(any.type)
	{
	case eMonoAnyType_Boolean:
		{
			return CreateMonoObject<bool>(any.b);
		}
		break;
	case eMonoAnyType_Integer:
		{
			return CreateMonoObject<int>((int)any.i);
		}
		break;
	case eMonoAnyType_UnsignedInteger:
		{
			return ToManagedType(eCMT_EntityId, new mono::entityId((EntityId)any.u));
		}
		break;
	case eMonoAnyType_Short:
		{
			return CreateMonoObject<short>((short)any.i);
		}
	case eMonoAnyType_UnsignedShort:
		{
			return CreateMonoObject<unsigned short>((unsigned short)any.u);
		}
		break;
	case eMonoAnyType_Float:
		{
			return CreateMonoObject<float>(any.f);
		}
		break;
	case eMonoAnyType_Vec3:
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