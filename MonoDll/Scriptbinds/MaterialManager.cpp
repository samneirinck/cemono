#include "StdAfx.h"
#include "MaterialManager.h"

#include <IGameFramework.h>

IMaterialManager *CScriptbind_MaterialManager::m_pMaterialManager = NULL;

CScriptbind_MaterialManager::CScriptbind_MaterialManager()
{
	m_pMaterialManager = gEnv->p3DEngine->GetMaterialManager();

	REGISTER_METHOD(MaterialExists);
	REGISTER_METHOD(CreateMaterial);
	REGISTER_METHOD(LoadMaterial);
}

bool CScriptbind_MaterialManager::MaterialExists(mono::string name)
{
	return m_pMaterialManager->FindMaterial(ToCryString(name));
}

void CScriptbind_MaterialManager::CreateMaterial(mono::string name)
{
	m_pMaterialManager->CreateMaterial(ToCryString(name));
}

void CScriptbind_MaterialManager::LoadMaterial(mono::string name, bool makeIfNotFound, bool nonRemovable)
{
	m_pMaterialManager->LoadMaterial(ToCryString(name), makeIfNotFound, nonRemovable);
}