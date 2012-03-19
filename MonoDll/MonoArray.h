/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// Wrapper for the MonoArray for less intensively ugly code and
// better workflow.
//////////////////////////////////////////////////////////////////////////
// 17/12/2011 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __MONO_ARRAY_H__
#define __MONO_ARRAY_H__

#include "MonoObject.h"

#include <IMonoArray.h>

class CScriptArray : public IMonoArray
{
public:
	// Used on MonoArray's returned from C#.
	CScriptArray(mono::array monoArray) : m_pArray(monoArray) { m_arrayHandle = mono_gchandle_new((MonoObject *)m_pArray, false); }

	// Used to send arrays to C#.
	CScriptArray(int size);
	virtual ~CScriptArray();

	// IMonoArray
	virtual void Release() override { delete this; }

	virtual void Clear() override { for(int i = 0; i < GetSize(); i++) mono_array_set((MonoArray *)m_pArray, void *, i, NULL);  }
	virtual int GetSize() const override { return (int)mono_array_length((MonoArray *)m_pArray); }

	virtual IMonoObject *GetItem(int index) override;
	virtual const char *GetItemString(int index) override { return ToCryString(mono_array_get((MonoArray *)m_pArray, mono::string , index)); }
	virtual IMonoArray *GetItemArray(int index) override { return new CScriptArray((mono::array)mono_array_get((MonoArray *)m_pArray, MonoArray *, index)); }

	virtual void Insert(IMonoObject *pObject) override;
	virtual void Insert(IMonoArray *pArray) override { pArray ? InsertArray(*pArray) : 0; }

	virtual void InsertString(mono::string string) override;
	virtual void InsertObject(mono::object obj) override;

	virtual void Insert(MonoAnyValue value) override;
	// ~IMonoArray

	virtual void InsertArray(mono::array arr);

	virtual mono::array GetMonoArray() override { return m_pArray; }

private:
	mono::array m_pArray;
	int curIndex;

	int m_arrayHandle;
};

#endif //__MONO_ARRAY_H__