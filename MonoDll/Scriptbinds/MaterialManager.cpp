#include "StdAfx.h"
#include "MaterialManager.h"

#include <IGameFramework.h>

IMaterialManager *CScriptbind_MaterialManager::m_pMaterialManager = NULL;

CScriptbind_MaterialManager::CScriptbind_MaterialManager()
{
	m_pMaterialManager = gEnv->p3DEngine->GetMaterialManager();

	REGISTER_METHOD(CreateMaterial);
	REGISTER_METHOD(LoadMaterial);

	REGISTER_METHOD(GetSurfaceTypeName);
}

IMaterial *CScriptbind_MaterialManager::CreateMaterial(mono::string name)
{
	return m_pMaterialManager->CreateMaterial(ToCryString(name));
}

IMaterial *CScriptbind_MaterialManager::LoadMaterial(mono::string name, bool makeIfNotFound, bool nonRemovable)
{
	return m_pMaterialManager->LoadMaterial(ToCryString(name), makeIfNotFound, nonRemovable);
}

mono::string CScriptbind_MaterialManager::GetSurfaceTypeName(IMaterial *pMaterial)
{
	const char *surfaceType = "";

	if(ISurfaceType *pSurfaceType = pMaterial->GetSurfaceType())
		surfaceType = pSurfaceType->GetName();

	return ToMonoString(surfaceType);
}