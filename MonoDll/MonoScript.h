#ifndef __MONO_SCRIPT_H__
#define __MONO_SCRIPT_H__

#include "Headers/IMonoScript.h"

struct IMonoArray;
struct IMonoResult;

class CMonoScript : public IMonoScript
{
public:
	CMonoScript(int _id, const char *scriptName, EMonoScriptType type);
	~CMonoScript();

	// IMonoScript
	virtual int GetId() override { return m_id; }
	virtual const char *GetName() override { return m_name; }
	virtual EMonoScriptType GetType() override { return m_type; }

	virtual void Release() override;

	virtual IMonoObject *InvokeMethod(const char *func, IMonoArray *pArgs = NULL) override;
	// ~IMonoScript

private:
	int m_id;
	const char *m_name;
	EMonoScriptType m_type;
};

#endif //__MONO_SCRIPT_H__