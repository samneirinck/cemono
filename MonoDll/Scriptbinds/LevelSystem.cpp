#include "StdAfx.h"
#include "LevelSystem.h"

#include <ILevelSystem.h>
#include <IGameFramework.h>

CLevelSystem::CLevelSystem()
{
	REGISTER_METHOD(GetCurrentLevel);
	REGISTER_METHOD(LoadLevel);
	REGISTER_METHOD(IsLevelLoaded);
	REGISTER_METHOD(UnloadLevel);

	REGISTER_METHOD(GetName);
	REGISTER_METHOD(GetPath);
	REGISTER_METHOD(GetPaks);
	REGISTER_METHOD(GetDisplayName);

	REGISTER_METHOD(GetHeightmapSize);

	REGISTER_METHOD(GetGameTypeCount);
	REGISTER_METHOD(GetGameType);
	REGISTER_METHOD(SupportsGameType);
	REGISTER_METHOD(GetDefaultGameType);

	REGISTER_METHOD(HasGameRules);
}

ILevelInfo *CLevelSystem::GetCurrentLevel()
{
	if(ILevel *pLevel = gEnv->pGameFramework->GetILevelSystem()->GetCurrentLevel())
		return pLevel->GetLevelInfo();

	return nullptr;
}

ILevelInfo *CLevelSystem::LoadLevel(mono::string name)
{
	if(ILevel *pLevel = gEnv->pGameFramework->GetILevelSystem()->LoadLevel(ToCryString(name)))
		return pLevel->GetLevelInfo();

	return nullptr;
}

bool CLevelSystem::IsLevelLoaded()
{
	return gEnv->pGameFramework->GetILevelSystem()->IsLevelLoaded();
}

void CLevelSystem::UnloadLevel()
{
	return gEnv->pGameFramework->GetILevelSystem()->UnLoadLevel();
}

mono::string CLevelSystem::GetName(ILevelInfo *pLevelInfo)
{
	return ToMonoString(pLevelInfo->GetName());
}

mono::string CLevelSystem::GetPath(ILevelInfo *pLevelInfo)
{
	return ToMonoString(pLevelInfo->GetPath());
}

mono::string CLevelSystem::GetPaks(ILevelInfo *pLevelInfo)
{
	return ToMonoString(pLevelInfo->GetPaks());
}

mono::string CLevelSystem::GetDisplayName(ILevelInfo *pLevelInfo)
{
	return ToMonoString(pLevelInfo->GetDisplayName());
}

int CLevelSystem::GetHeightmapSize(ILevelInfo *pLevelInfo)
{
	return pLevelInfo->GetHeightmapSize();
}
	
int CLevelSystem::GetGameTypeCount(ILevelInfo *pLevelInfo)
{
	return pLevelInfo->GetGameTypeCount();
}

mono::string CLevelSystem::GetGameType(ILevelInfo *pLevelInfo, int gameType)
{
	if(auto pGameType = pLevelInfo->GetGameType(gameType))
		return ToMonoString(pGameType->name);

	return ToMonoString("");
}

bool CLevelSystem::SupportsGameType(ILevelInfo *pLevelInfo, mono::string gameTypeName)
{
	return pLevelInfo->SupportsGameType(ToCryString(gameTypeName));
}

mono::string CLevelSystem::GetDefaultGameType(ILevelInfo *pLevelInfo)
{
	if(auto pGameType = pLevelInfo->GetDefaultGameType())
		return ToMonoString(pGameType->name);

	return ToMonoString("");
}

bool CLevelSystem::HasGameRules(ILevelInfo *pLevelInfo)
{
	return pLevelInfo->HasGameRules();
}