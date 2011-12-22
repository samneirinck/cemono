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

	virtual void Release() = 0;

	virtual IMonoObject *InvokeMethod(const char *func, IMonoArray *pArgs = NULL) = 0;
};

typedef int MonoScriptId;

#endif //__IMONOSCRIPT_H__