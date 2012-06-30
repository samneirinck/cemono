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

class CScriptbind_ItemSystem : public IMonoScriptBind
{
public:
	CScriptbind_ItemSystem();
	~CScriptbind_ItemSystem() {}

protected:
	// IMonoScriptBind
	virtual const char *GetClassName() override { return "NativeItemSystemMethods"; }
	// ~IMonoScriptBind

	static void CacheItemGeometry(mono::string);
	static void CacheItemSound(mono::string);

	static void GiveItem(EntityId, mono::string);
	static void GiveEquipmentPack(EntityId, mono::string);
};

#endif //__SCRIPTBIND_ITEMSYSTEM_H__