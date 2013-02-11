#ifndef __DYN_MONO_ARRAY_H__
#define __DYN_MONO_ARRAY_H__

#include "MonoArray.h"

class CDynScriptArray
	: public CScriptArray
{
public:
	CDynScriptArray(MonoDomain *pDomain, IMonoClass *pContainingType = nullptr, int size = 0);
	CDynScriptArray(mono::object monoArray) : CScriptArray(monoArray) {}

	// CScriptArray
	virtual void Clear() override;

	virtual void Remove(int index) override;

	virtual IMonoArray *Clone() override { return new CDynScriptArray((mono::object)mono_array_clone((MonoArray *)m_pObject)); }

	virtual inline void Insert(mono::object object, int index = -1) override;

	virtual void InsertNativePointer(void *ptr, int index = -1) override;
	virtual void InsertObject(IMonoObject *pObject, int index = -1) override;
	virtual void InsertAny(MonoAnyValue value, int index = -1) override;
	virtual void InsertMonoString(mono::string string, int index = -1) override { Insert((mono::object)string, index); }
	// ~CScriptArray
};

#endif // __DYN_MONO_ARRAY_H__