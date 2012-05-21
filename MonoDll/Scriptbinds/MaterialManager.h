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
public:
	CScriptbind_MaterialManager();
	~CScriptbind_MaterialManager() {}

protected:

	// Externals
	static IMaterial *CreateMaterial(mono::string name);
	static IMaterial *LoadMaterial(mono::string name, bool makeIfNotFound, bool nonRemovable);

	static IMaterial *GetSubMaterial(IMaterial *pMaterial, int slot);

	static IMaterial *GetMaterial(IEntity *pEnt, int slot);
	static void SetMaterial(IEntity *pEnt, IMaterial *pMaterial);

	static mono::string GetSurfaceTypeName(IMaterial *pMaterial);

	static bool SetGetMaterialParamFloat(IMaterial *pMaterial, mono::string paramName, float &v, bool get);
	static bool SetGetMaterialParamVec3(IMaterial *pMaterial, mono::string paramName, Vec3 &v, bool get);

	static EMaterialFlags GetFlags(IMaterial *pMaterial);
	static void SetFlags(IMaterial *pMaterial, EMaterialFlags flags);

	static void SetShaderParam(IMaterial *pMaterial, mono::string monoParamName, float newVal);
	// ~Externals

	// IMonoScriptBind
	virtual const char *GetClassName() override { return "Material"; }
	// ~IMonoScriptBind

	static IMaterialManager *m_pMaterialManager;
};

#endif //__SCRIPTBIND_MATERIAL_MANAGER__