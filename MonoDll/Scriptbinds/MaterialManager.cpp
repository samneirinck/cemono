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

mono::string CScriptbind_MaterialManager::GetSurfaceTypeName(int matId)
{
	const char *surfaceType = "";

	if(ISurfaceType *pSurfaceType = m_materials[matId]->GetSurfaceType())
		surfaceType = pSurfaceType->GetName();

	return ToMonoString(surfaceType);
}

int CScriptbind_MaterialManager::CreateMaterial(mono::string name)
{
	if(IMaterial *pMaterial = m_pMaterialManager->CreateMaterial(ToCryString(name)))
	{
		int index = m_materials.size();
		m_materials.insert(TMaterialMap::value_type(index, pMaterial));

		return index;
	}

	return -1;
}

int CScriptbind_MaterialManager::LoadMaterial(mono::string name, bool makeIfNotFound, bool nonRemovable)
{
	for each(auto material in m_materials)
	{
		if(!strcmp(material.second->GetName(), ToCryString(name)))
			return material.first;
	}

	if(IMaterial *pMaterial = m_pMaterialManager->LoadMaterial(ToCryString(name), makeIfNotFound, nonRemovable))
	{
		int index = m_materials.size();
		m_materials.insert(TMaterialMap::value_type(index, pMaterial));

		return index;
	}

	return -1;
}