#ifndef __MONOCVARS_H__
#define __MONOCVARS_H__

struct SCVars
{
	int mono_exceptionsTriggerMessageBoxes;
	int mono_exceptionsTriggerFatalErrors;

	int mono_realtimeScripting;
	int mono_realtimeScriptingDebug;
	int mono_realtimeScriptingDetectChanges;

	int mono_softBreakpoints;

	SCVars()
	{
		memset(this,0,sizeof(SCVars));
		InitCVars(gEnv->pConsole);
	}

	~SCVars() { ReleaseCVars(); }

	void InitCVars(IConsole *pConsole);
	void ReleaseCVars();
};

#endif //__MONOCVARS_H__