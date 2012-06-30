#ifndef __SCRIPTBIND_TIME_H__
#define __SCRIPTBIND_TIME_H__

#include <IMonoScriptBind.h>

class CTime : public IMonoScriptBind
{
public:
	CTime()
	{
		REGISTER_METHOD(SetTimeScale);
	}

	~CTime() {}

protected:
	// IMonoScriptBind
	virtual const char *GetClassName() { return "NativeTimeMethods"; }
	// ~IMonoScriptBind

	static void SetTimeScale(float scale)
	{
		gEnv->pTimer->SetTimeScale(scale);
	}
};

#endif //__SCRIPTBIND_TIME_H__