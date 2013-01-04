#ifndef __DYN_MONO_ARRAY_H__
#define __DYN_MONO_ARRAY_H__

#include "MonoArray.h"

class CDynScriptArray
	: public CScriptArray
{
public:
	CDynScriptArray(IMonoClass *pContainingType = nullptr, int size = 0);
	CDynScriptArray(mono::object monoArray) : CScriptArray(monoArray) {}

	// CScriptArray
	virtual void Clear() override;

	virtual void Remove(int index) override;

	virtual IMonoArray *Clone() override { return new CDynScriptArray((mono::object)mono_array_clone((MonoArray *)m_pObject)); }

	virtual inline void Insert(mono::object object, int index = -1) override;
	// ~CScriptArray
};

#endif // __DYN_MONO_ARRAY_H__