#ifndef __SCRIPTBIND_ACTORSYSTEM_H__
#define __SCRIPTBIND_ACTORSYSTEM_H__

#include "Headers\IMonoScriptBind.h"

class CScriptBind_ActorSystem : public IMonoScriptBind
{
public:
	CScriptBind_ActorSystem();
	~CScriptBind_ActorSystem() {}

protected:
	// IMonoScriptBind
	virtual const char* GetClassName() { return "ActorSystem"; }
	// ~IMonoScriptBind
};

#endif //__SCRIPTBIND_ACTORSYSTEM_H__