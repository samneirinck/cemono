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

class CScriptArray 
	: public CScriptObject
	, public IMonoArray
{
public:
	// Used on MonoArray's returned from C#.
	CScriptArray(mono::object monoArray);
	// Used to send arrays to C#.
	CScriptArray(int size);

	virtual ~CScriptArray();

	// IMonoArray
	virtual void Clear() override { for(int i = 0; i < GetSize(); i++) mono_array_set((MonoArray *)m_pObject, void *, i, NULL);  }

	virtual void Resize(int size);
	virtual int GetSize() const override { return (int)mono_array_length((MonoArray *)m_pObject); }

	virtual IMonoObject *GetItem(int index) override;
	virtual const char *GetItemString(int index) override { return ToCryString(mono_array_get((MonoArray *)m_pObject, mono::string , index)); }

	virtual void InsertNativePointer(void *ptr, int index = -1) override;
	virtual void InsertObject(IMonoObject *pObject, int index = -1) override;
	virtual void InsertAny(MonoAnyValue value, int index = -1) override;
	virtual void InsertMonoString(mono::string string, int index = -1);
	// ~IMonoArray

	// IMonoObject
	virtual void Release() override { delete this; };

	virtual IMonoObject *CallMethod(const char *methodName, IMonoArray *params = NULL, bool bStatic = false) override { return CScriptObject::CallMethod(methodName, params, bStatic); }

	virtual IMonoObject *GetProperty(const char *propertyName, bool bStatic = false) override { return CScriptObject::GetProperty(propertyName, bStatic); }
	virtual void SetProperty(const char *propertyName, IMonoObject *pNewValue, bool bStatic = false) override { CScriptObject::SetProperty(propertyName, pNewValue, bStatic); }
	virtual IMonoObject *GetField(const char *fieldName, bool bStatic = false) override { return CScriptObject::GetField(fieldName, bStatic); }
	virtual void SetField(const char *fieldName, IMonoObject *pNewValue, bool bStatic = false) override { CScriptObject::SetField(fieldName, pNewValue, bStatic); }

	virtual EMonoAnyType GetType() override { return eMonoAnyType_Array; }

	virtual mono::object GetManagedObject() override { return CScriptObject::GetManagedObject(); }

	virtual IMonoClass *GetClass() override { return CScriptObject::GetClass(); }

	virtual void *UnboxObject() override { return CScriptObject::UnboxObject(); }
	// ~IMonoObject

	virtual void InsertMonoArray(mono::object arr, int index = -1);
	virtual void InsertMonoObject(mono::object object, int index = -1);

private:
	int m_curIndex;
};

#endif //__MONO_ARRAY_H__