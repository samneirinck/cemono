#include "StdAfx.h"
#include "MonoConverter.h"

#include <MonoArray.h>
#include <MonoAssembly.h>
#include "MonoClass.h"

#include <MonoAnyValue.h>

IMonoArray *CConverter::CreateArray(int numArgs)
{
	if(numArgs < 1)
	{
		MonoWarning("Attempted to create array with invalid size %i", numArgs);
		return NULL;
	}

	return new CScriptArray(numArgs); 
}

IMonoArray *CConverter::ToArray(mono::object arr)
{
	if(arr == NULL)
	{
		MonoWarning("Failed to convert mono::array");
		return NULL;
	}

	return new CScriptArray(arr);
}

IMonoObject *CConverter::ToObject(mono::object obj)
{
	if(obj == NULL)
	{
		MonoWarning("Failed to convert mono::object");
		return NULL;
	}

	return new CScriptObject((MonoObject *)obj);
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
			if(IMonoAssembly *pCryBraryAssembly = gEnv->pMonoScriptSystem->GetCryBraryAssembly())
			{
				if(IMonoClass *pClass = pCryBraryAssembly->GetClass("EntityId"))
				{
					IMonoArray *pArgs = CreateMonoArray(1);
					pArgs->Insert((int)any.u);

					IMonoObject *pInstance = pClass->CreateInstance(pArgs);
					SAFE_RELEASE(pClass);
					return pInstance;
				}
			}
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