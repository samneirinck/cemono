/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// IMonoScript interface for external projects, i.e. CryGame.
//////////////////////////////////////////////////////////////////////////
// 02/12/2011 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __IMONOSCRIPT_H__
#define __IMONOSCRIPT_H__

enum EMonoScriptType;

struct IMonoArray;
struct IMonoObject;

struct IMonoScript
{
	virtual int GetId() = 0;
	virtual const char *GetName() = 0;
	virtual EMonoScriptType GetType() = 0;

	virtual IMonoObject *InvokeMethod(const char *func, IMonoArray *pArgs = NULL) = 0;
	template <typename T>
	T InvokeMethod(const char *func, IMonoArray *pArgs = NULL)
	{
		if(IMonoObject *pObject = InvokeMethod(func, pArgs))
			return pObject->Unbox<T>();

		return (T)0;
	}
};

typedef int MonoScriptId;

#endif //__IMONOSCRIPT_H__