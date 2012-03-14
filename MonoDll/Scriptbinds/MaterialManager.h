///////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// IMaterialManager scriptbind
//////////////////////////////////////////////////////////////////////////
// 14/03/2012 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/

#ifndef __SCRIPTBIND_MATERIAL_MANAGER__
#define __SCRIPTBIND_MATERIAL_MANAGER__

#include <MonoCommon.h>
#include <IMonoScriptBind.h>

struct IMaterialManager;

class CScriptbind_MaterialManager : public IMonoScriptBind
{
	typedef std::map<const char *, int> TMaterialMap;

public:
	CScriptbind_MaterialManager();
	~CScriptbind_MaterialManager() {}

protected:

	// Externals
	static bool MaterialExists(mono::string name);

	static void CreateMaterial(mono::string name);
	static void LoadMaterial(mono::string name, bool makeIfNotFound, bool nonRemovable);
	// ~Externals

	// IMonoScriptBind
	virtual const char *GetClassName() override { return "Material"; }
	// ~IMonoScriptBind

	static IMaterialManager *m_pMaterialManager;
};

#endif //__SCRIPTBIND_MATERIAL_MANAGER__