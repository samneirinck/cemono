#include "StdAfx.h"
#include "MaterialManager.h"

#include <IGameFramework.h>

IMaterialManager *CScriptbind_MaterialManager::m_pMaterialManager = NULL;
CScriptbind_MaterialManager::TMaterialMap CScriptbind_MaterialManager::m_materials = CScriptbind_MaterialManager::TMaterialMap();

CScriptbind_MaterialManager::CScriptbind_MaterialManager()
{
	m_pMaterialManager = gEnv->p3DEngine->GetMaterialManager();

	REGISTER_METHOD(CreateMaterial);
	REGISTER_METHOD(LoadMaterial);
}

int CScriptbind_MaterialManager::CreateMaterial(mono::string name)
{
	if(IMaterial *pMaterial = m_pMaterialManager->CreateMaterial(ToCryString(name)))
	{
		int index = m_materials.size();
		m_materials.insert(TMaterialMap::value_type(pMaterial, index));

		return index;
	}

	return -1;
}

int CScriptbind_MaterialManager::LoadMaterial(mono::string name, bool makeIfNotFound, bool nonRemovable)
{
	if(IMaterial *pMaterial = m_pMaterialManager->LoadMaterial(ToCryString(name), makeIfNotFound, nonRemovable))
	{
		int index = m_materials.size();
		m_materials.insert(TMaterialMap::value_type(pMaterial, index));

		return index;
	}

	return -1;
}