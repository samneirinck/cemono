#ifndef __CRYSCRIPTINSTANCE_H__
#define __CRYSCRIPTINSTANCE_H__

#include "MonoScriptSystem.h"
#include "MonoObject.h"

class CCryScriptInstance 
	: public CScriptObject
	, public IMonoScriptEventListener
{
public:
	CCryScriptInstance(mono::object scriptInstance);
	~CCryScriptInstance();

	// IMonoScriptEventListener
	virtual void OnReloadStart() {}
	virtual void OnReloadComplete();
	// ~IMonoScriptEventListener

private:
	int m_scriptId;
};

#endif //__CRYSCRIPTINSTANCE_H__