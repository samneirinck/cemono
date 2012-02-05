/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// ItemSystem scriptbind, used for various item-specific events i.e.
// item caching.
//////////////////////////////////////////////////////////////////////////
// 20/11/2011 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __SCRIPTBIND_ITEMSYSTEM_H__
#define __SCRIPTBIND_ITEMSYSTEM_H__

#include <IMonoScriptBind.h>
#include <MonoCommon.h>

class CScriptBind_ItemSystem : public IMonoScriptBind
{
public:
	CScriptBind_ItemSystem();
	~CScriptBind_ItemSystem() {}

protected:
	// IMonoScriptBind
	virtual const char *GetClassName() { return "ItemSystem"; }
	// ~IMonoScriptBind

	ExposedMonoMethod(void, CacheItemGeometry, mono::string);
	ExposedMonoMethod(void, CacheItemSound, mono::string);
};

class CScriptBind_Inventory : public IMonoScriptBind
{
public:
	CScriptBind_Inventory();
	~CScriptBind_Inventory() {}

protected:
	// IMonoScriptBind
	virtual const char *GetClassName() { return "Inventory"; }
	// ~IMonoScriptBind

	MonoMethod(void, GiveItem, EntityId, mono::string);
	MonoMethod(void, GiveEquipmentPack, EntityId, mono::string);
};

#endif //__SCRIPTBIND_ITEMSYSTEM_H__