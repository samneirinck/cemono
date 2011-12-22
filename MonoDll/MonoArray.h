#ifndef __MONO_ARRAY_H__
#define __MONO_ARRAY_H__

#include "MonoString.h"
#include <IMonoArray.h>

class CMonoArray : public IMonoArray
{
public:
	// Used on MonoArray's returned from C#.
	CMonoArray(MonoArray *pMonoArray) : m_pArray(pMonoArray) { }

	// Used to send arrays to C#.
	CMonoArray(int size);
	virtual ~CMonoArray() { }

	// IMonoArray
	virtual void Clear() override { for(int i = 0; i < GetSize(); i++) mono_array_set(m_pArray, void *, i, NULL);  }
	virtual int GetSize() const override { return (int)mono_array_length(m_pArray); }

	virtual IMonoObject *GetItem(int index) override;
	virtual const char *GetItemString(int index) override { return ToCryString(mono_array_get(m_pArray, MonoString *, index)); }

	virtual void InsertObject(IMonoObject *pObject) override;
	virtual void InsertString(const char *string) override { InsertString(ToMonoString(string)); }
	virtual void InsertArray(IMonoArray *pArray) override { InsertArray(static_cast<CMonoArray *>(pArray)->GetMonoArray()); }

	// ~IMonoArray

	void InsertObject(MonoObject *pObject);
	void InsertString(MonoString *pString);
	void InsertArray(MonoArray *pArray);

	virtual MonoArray *GetMonoArray() { return m_pArray; }

private:
	MonoArray *m_pArray;
	int curIndex;
};

static IMonoArray *CreateMonoArray(int numArgs)
{
	if(numArgs<1)
		return NULL;

	return new CMonoArray(numArgs);
}

#endif //__MONO_ARRAY_H__