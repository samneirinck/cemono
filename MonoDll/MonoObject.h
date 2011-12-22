#ifndef __MONO_OBJECT_H__
#define __MONO_OBJECT_H__

#include "MonoString.h"

#include "MonoScriptSystem.h"

#include <IMonoObject.h>
#include <mono/mini/jit.h>

class CMonoObject : public IMonoObject
{
public:
	CMonoObject(MonoObject *pObject) { m_pObject = pObject; m_objectHandle = mono_gchandle_new(pObject, true); }
	virtual ~CMonoObject() { mono_gchandle_free(m_objectHandle); }

	MonoClass *GetMonoClass() { return mono_object_get_class(m_pObject); }
	MonoObject *GetMonoObject() { return m_pObject; }

private:
	// IMonoObject
	virtual void *UnboxObject() override { return mono_object_unbox(m_pObject); }
	// ~IMonoObject

protected:
	MonoObject *m_pObject;

	int m_objectHandle;
};

template <typename T>
static MonoClass *GetMonoClassOfType(T t)
{
	if(std::is_same<T, int>::value)
		return mono_get_int32_class();
	else if(std::is_same<T, float>::value)
		return mono_get_single_class();
	else if(std::is_same<T, bool>::value)
		return mono_get_boolean_class();
	else if(std::is_same<T, unsigned int>::value)
		return mono_get_uint32_class();
	else if(std::is_same<T, Vec3>::value || std::is_same<T, Ang3>::value)
	{
		CMonoScriptSystem *pScriptSystem = static_cast<CMonoScriptSystem *>(gEnv->pMonoScriptSystem);

		return static_cast<CMonoClass *>(pScriptSystem->GetCustomClass("Vec3"))->GetClass();
	}
	else if(std::is_same<T, MonoString *>::value)
		return mono_get_string_class();

	return mono_get_object_class();
}

template <typename T>
static IMonoObject *CreateMonoObject(T t)
{
	if(std::is_same<T, MonoString *>::value)
		CryLogAlways("[Warning] Trying to create MonoObject using a MonoString");
	if(std::is_same<T, const char *>::value)
		CryLogAlways("[Warning] Trying to create MonoObject using string");//return mono_value_box(mono_domain_get(), mono_get_string_class(), ToMonoString((const char *)t));
	else
		return new CMonoObject(mono_value_box(mono_domain_get(), GetMonoClassOfType<T>(t), &t));

	return NULL;
}

#endif //__MONO_OBJECT_H__