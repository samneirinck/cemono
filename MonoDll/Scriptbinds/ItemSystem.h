#ifndef __SCRIPTBIND_ITEMSYSTEM_H__
#define __SCRIPTBIND_ITEMSYSTEM_H__

#include "Headers\IMonoScriptBind.h"

class CScriptBind_ItemSystem : public IMonoScriptBind
{
public:
	CScriptBind_ItemSystem();
	~CScriptBind_ItemSystem() {}

protected:
	// IMonoScriptBind
	virtual const char* GetClassName() { return "ItemSystem"; }
	// ~IMonoScriptBind

	static void CacheItemGeometry(MonoString *itemClass);
	static void CacheItemSound(MonoString *itemClass);
};

#endif //__SCRIPTBIND_ITEMSYSTEM_H__