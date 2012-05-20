#include "StdAfx.h"
#include "MaterialManager.h"

#include <IGameFramework.h>

IMaterialManager *CScriptbind_MaterialManager::m_pMaterialManager = NULL;

CScriptbind_MaterialManager::CScriptbind_MaterialManager()
{
	m_pMaterialManager = gEnv->p3DEngine->GetMaterialManager();

	REGISTER_METHOD(CreateMaterial);
	REGISTER_METHOD(LoadMaterial);

	REGISTER_METHOD(GetSubMaterial);

	REGISTER_METHOD(GetMaterial);
	REGISTER_METHOD(SetMaterial);

	REGISTER_METHOD(GetSurfaceTypeName);

	REGISTER_METHOD(SetGetMaterialParamFloat);
	REGISTER_METHOD(SetGetMaterialParamVec3);

	REGISTER_METHOD(SetShaderParam);
}

IMaterial *CScriptbind_MaterialManager::CreateMaterial(mono::string name)
{
	return m_pMaterialManager->CreateMaterial(ToCryString(name));
}

IMaterial *CScriptbind_MaterialManager::LoadMaterial(mono::string name, bool makeIfNotFound, bool nonRemovable)
{
	return m_pMaterialManager->LoadMaterial(ToCryString(name), makeIfNotFound, nonRemovable);
}

IMaterial *CScriptbind_MaterialManager::GetSubMaterial(IMaterial *pMaterial, int slot)
{
	return pMaterial->GetSubMtl(slot);
}

IMaterial *CScriptbind_MaterialManager::GetMaterial(IEntity *pEntity, int slot)
{
	SEntitySlotInfo slotInfo;
	if(pEntity->GetSlotInfo(slot, slotInfo))
		return slotInfo.pMaterial;

	return NULL;
}

void CScriptbind_MaterialManager::SetMaterial(IEntity *pEntity, IMaterial *pMaterial)
{
	pEntity->SetMaterial(pMaterial);
}

mono::string CScriptbind_MaterialManager::GetSurfaceTypeName(IMaterial *pMaterial)
{
	const char *surfaceType = "";

	if(ISurfaceType *pSurfaceType = pMaterial->GetSurfaceType())
		surfaceType = pSurfaceType->GetName();

	return ToMonoString(surfaceType);
}

bool CScriptbind_MaterialManager::SetGetMaterialParamFloat(IMaterial *pMaterial, mono::string paramName, float &v, bool get)
{
	return pMaterial->SetGetMaterialParamFloat(ToCryString(paramName), v, get);
}

bool CScriptbind_MaterialManager::SetGetMaterialParamVec3(IMaterial *pMaterial, mono::string paramName, Vec3 &v, bool get)
{
	return pMaterial->SetGetMaterialParamVec3(ToCryString(paramName), v, get);
}

void CScriptbind_MaterialManager::SetShaderParam(IMaterial *pMaterial, mono::string monoParamName, float newVal)
{
	const SShaderItem& shaderItem(pMaterial->GetShaderItem());
	DynArray<SShaderParam> params;
	params = shaderItem.m_pShader->GetPublicParams();

	const char *paramName = ToCryString(monoParamName);

	for (DynArray<SShaderParam>::iterator it = params.begin(), end = params.end(); it != end; ++it)
	{
		SShaderParam param = *it;

		if(!strcmp(paramName, param.m_Name))
		{
			UParamVal par;
			par.m_Float = newVal;

			param.SetParam(paramName, &params, par);

			SInputShaderResources res;
			shaderItem.m_pShaderResources->ConvertToInputResource(&res);
			res.m_ShaderParams = params;
			shaderItem.m_pShaderResources->SetShaderParams(&res,shaderItem.m_pShader);
			break;
		}
	}
}