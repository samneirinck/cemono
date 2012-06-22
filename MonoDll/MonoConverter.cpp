#include "StdAfx.h"
#include "MonoConverter.h"

#include <MonoArray.h>
#include <MonoAssembly.h>
#include "MonoClass.h"

#include <MonoAnyValue.h>

IMonoArray *CConverter::CreateArray(int numArgs)
{
	return new CScriptArray(numArgs); 
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
		return CreateMonoObject<bool>(any.b);
	case eMonoAnyType_Integer:
		return CreateMonoObject<int>(any.i);
	case eMonoAnyType_UnsignedInteger:
		return CreateMonoObject<unsigned int>(any.u);
	case eMonoAnyType_Short:
		return CreateMonoObject<short>((short)any.i);
	case eMonoAnyType_UnsignedShort:
		return CreateMonoObject<unsigned short>((unsigned short)any.u);
	case eMonoAnyType_Float:
		return CreateMonoObject<float>(any.f);
	case eMonoAnyType_Vec3:
		{
			if(IMonoAssembly *pCryBraryAssembly = gEnv->pMonoScriptSystem->GetCryBraryAssembly())
			{
				if(IMonoClass *pClass = pCryBraryAssembly->GetClass("Vec3"))
				{
					IMonoArray *pArgs = CreateMonoArray(3);
					pArgs->Insert(any.vec3.x);
					pArgs->Insert(any.vec3.y);
					pArgs->Insert(any.vec3.z);

					IMonoObject *pInstance = pClass->CreateInstance(pArgs);
					SAFE_RELEASE(pClass);
					return pInstance;
				}
			}
		}
		break;
	}

	return NULL;
}