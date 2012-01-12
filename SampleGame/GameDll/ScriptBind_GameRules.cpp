/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2004.
-------------------------------------------------------------------------
$Id$
$DateTime$

-------------------------------------------------------------------------
History:
- 27:10:2004   11:29 : Created by Márcio Martins

*************************************************************************/
#include "StdAfx.h"
#include "ScriptBind_GameRules.h"
#include "GameRules.h"
#include "Actor.h"
#include "Game.h"
#include "GameCVars.h"
#include "Player.h"

#include <MonoCommon.h>
#include <IMonoArray.h>

//------------------------------------------------------------------------
CScriptBind_GameRules::CScriptBind_GameRules(ISystem *pSystem, IGameFramework *pGameFramework)
{
	RegisterMethods();

	gEnv->pMonoScriptSystem->RegisterScriptBind(this);
}

//------------------------------------------------------------------------
CScriptBind_GameRules::~CScriptBind_GameRules()
{
}

//------------------------------------------------------------------------
void CScriptBind_GameRules::RegisterMethods()
{
	REGISTER_METHOD(SpawnPlayer);
	REGISTER_METHOD(RevivePlayer);
	REGISTER_METHOD(RevivePlayerInVehicle);
	REGISTER_METHOD(RenamePlayer);
	REGISTER_METHOD(KillPlayer);
	REGISTER_METHOD(MovePlayer);
	REGISTER_METHOD(GetPlayerByChannelId);
	REGISTER_METHOD(GetChannelId);
	REGISTER_METHOD(GetPlayerCount);
	REGISTER_METHOD(GetSpectatorCount);
	REGISTER_METHOD(GetPlayers);
	REGISTER_METHOD(IsPlayerInGame);
	REGISTER_METHOD(IsProjectile);
	REGISTER_METHOD(IsSameTeam);
	REGISTER_METHOD(IsNeutral);
	
	REGISTER_METHOD(AddSpawnLocation);
	REGISTER_METHOD(RemoveSpawnLocation);
	REGISTER_METHOD(GetSpawnLocationCount);
	REGISTER_METHOD(GetSpawnLocationByIdx);
	REGISTER_METHOD(GetSpawnLocations);
	REGISTER_METHOD(GetSpawnLocation);
	REGISTER_METHOD(GetFirstSpawnLocation);

	REGISTER_METHOD(AddSpawnGroup);
	REGISTER_METHOD(AddSpawnLocationToSpawnGroup);
	REGISTER_METHOD(RemoveSpawnLocationFromSpawnGroup);
	REGISTER_METHOD(RemoveSpawnGroup);
	REGISTER_METHOD(GetSpawnLocationGroup);
	REGISTER_METHOD(GetSpawnGroups);
	REGISTER_METHOD(IsSpawnGroup);

	REGISTER_METHOD(GetTeamDefaultSpawnGroup);
	REGISTER_METHOD(SetTeamDefaultSpawnGroup);
	REGISTER_METHOD(SetPlayerSpawnGroup);

	REGISTER_METHOD(AddSpectatorLocation);
	REGISTER_METHOD(RemoveSpectatorLocation);
	REGISTER_METHOD(GetSpectatorLocationCount);
	REGISTER_METHOD(GetSpectatorLocation);
	REGISTER_METHOD(GetSpectatorLocations);
	REGISTER_METHOD(GetRandomSpectatorLocation);
	REGISTER_METHOD(GetInterestingSpectatorLocation);
	REGISTER_METHOD(GetNextSpectatorTarget);
	REGISTER_METHOD(ChangeSpectatorMode);
	REGISTER_METHOD(CanChangeSpectatorMode);

	REGISTER_METHOD(AddMinimapEntity);
	REGISTER_METHOD(RemoveMinimapEntity);
	REGISTER_METHOD(ResetMinimap);

	REGISTER_METHOD(GetPing);
	REGISTER_METHOD(ResetEntities);

	REGISTER_METHOD(ServerExplosion);
	REGISTER_METHOD(ServerHit);

	REGISTER_METHOD(CreateTeam);
	REGISTER_METHOD(RemoveTeam);
	REGISTER_METHOD(GetTeamName);
	REGISTER_METHOD(GetTeamId);
	REGISTER_METHOD(GetTeamCount);
	REGISTER_METHOD(GetTeamPlayerCount);
	REGISTER_METHOD(GetTeamChannelCount);
	REGISTER_METHOD(GetTeamPlayers);

	REGISTER_METHOD(SetTeam);
	REGISTER_METHOD(GetTeam);
	REGISTER_METHOD(GetChannelTeam);

	REGISTER_METHOD(AddObjective);
	REGISTER_METHOD(SetObjectiveStatus);
	REGISTER_METHOD(SetObjectiveEntity);
	REGISTER_METHOD(RemoveObjective);
	REGISTER_METHOD(ResetObjectives);
	/*
	REGISTER_METHOD(TextMessage);
	REGISTER_METHOD(SendTextMessage);
	REGISTER_METHOD(SendChatMessage);
	*/
	REGISTER_METHOD(ForbiddenAreaWarning);

	REGISTER_METHOD(ResetGameTime);
	REGISTER_METHOD(GetRemainingGameTime);
	REGISTER_METHOD(IsTimeLimited);

	REGISTER_METHOD(ResetRoundTime);
	REGISTER_METHOD(GetRemainingRoundTime);
	REGISTER_METHOD(IsRoundTimeLimited);

	REGISTER_METHOD(ResetPreRoundTime);
	REGISTER_METHOD(GetRemainingPreRoundTime);

	REGISTER_METHOD(ResetReviveCycleTime);
	REGISTER_METHOD(GetRemainingReviveCycleTime);

	REGISTER_METHOD(ResetGameStartTimer);
	REGISTER_METHOD(GetRemainingStartTimer);

	REGISTER_METHOD(EndGame);
	REGISTER_METHOD(NextLevel);

	REGISTER_METHOD(GetHitMaterialId);
	REGISTER_METHOD(GetHitMaterialName);
	REGISTER_METHOD(ResetHitMaterials);

	REGISTER_METHOD(RegisterHitType);
	REGISTER_METHOD(GetHitTypeId);
	REGISTER_METHOD(GetHitType);
	REGISTER_METHOD(ResetHitTypes);

	REGISTER_METHOD(ForceScoreboard);
	REGISTER_METHOD(FreezeInput);

	REGISTER_METHOD(ScheduleEntityRespawn);
	REGISTER_METHOD(AbortEntityRespawn);

	REGISTER_METHOD(ScheduleEntityRemoval);
	REGISTER_METHOD(AbortEntityRemoval);
	/*
	REGISTER_METHOD(SetSynchedGlobalValue);
	REGISTER_METHOD(GetSynchedGlobalValue);

	REGISTER_METHOD(SetSynchedEntityValue);
	REGISTER_METHOD(GetSynchedEntityValue);

	REGISTER_METHOD(ResetSynchedStorage);
	REGISTER_METHOD(ForceSynchedStorageSynch);
	*/
	REGISTER_METHOD(IsDemoMode);
	REGISTER_METHOD(GetTimeLimit);
	REGISTER_METHOD(GetPreRoundTime);
	REGISTER_METHOD(GetRoundTime);
	REGISTER_METHOD(GetRoundLimit);
	REGISTER_METHOD(GetFragLimit);
	REGISTER_METHOD(GetFragLead);
  REGISTER_METHOD(GetFriendlyFireRatio);
  REGISTER_METHOD(GetReviveTime);
	REGISTER_METHOD(GetMinPlayerLimit);
	REGISTER_METHOD(GetMinTeamLimit);
	REGISTER_METHOD(GetTeamLock);
	
	REGISTER_METHOD(IsFrozen);
	REGISTER_METHOD(FreezeEntity);
	REGISTER_METHOD(ShatterEntity);

#ifndef _RELEASE
  REGISTER_METHOD(DebugCollisionDamage);
	REGISTER_METHOD(DebugHits);
#endif

	REGISTER_METHOD(SendHitIndicator);
	REGISTER_METHOD(SendDamageIndicator);
	/*
	REGISTER_METHOD(IsInvulnerable);
	REGISTER_METHOD(SetInvulnerability);
	*/
	REGISTER_METHOD(GameOver);
	REGISTER_METHOD(EnteredGame);
	REGISTER_METHOD(EndGameNear);

	REGISTER_METHOD(SPNotifyPlayerKill);

	REGISTER_METHOD(ProcessEMPEffect);
	REGISTER_METHOD(PerformDeadHit);

	REGISTER_METHOD(RegisterGameMode);
	REGISTER_METHOD(AddGameModeAlias);
	REGISTER_METHOD(AddGameModeLevelLocation);
	REGISTER_METHOD(SetDefaultGameMode);
}

//------------------------------------------------------------------------
CGameRules *CScriptBind_GameRules::GetGameRules()
{
	return static_cast<CGameRules *>(gEnv->pGameFramework->GetIGameRulesSystem()->GetCurrentGameRules());
}

//------------------------------------------------------------------------
CActor *CScriptBind_GameRules::GetActor(EntityId id)
{
	return static_cast<CActor *>(gEnv->pGameFramework->GetIActorSystem()->GetActor(id));
}

//------------------------------------------------------------------------
EntityId CScriptBind_GameRules::SpawnPlayer(int channelId, mono::string name, mono::string className, Vec3 pos, Vec3 angles)
{
	return g_pGame->GetGameRules()->SpawnPlayer(channelId, *name, *className, pos, Ang3(angles))->GetEntityId();
}

//------------------------------------------------------------------------
void CScriptBind_GameRules::RevivePlayer(EntityId playerId, Vec3 pos, Vec3 rot, int teamId, bool clearInventory)
{
	IActor *pActor = gEnv->pGameFramework->GetIActorSystem()->GetActor(playerId);
	if(!pActor)
		return;

	g_pGame->GetGameRules()->RevivePlayer(pActor, pos, Ang3(rot), teamId, clearInventory);
}

//------------------------------------------------------------------------
void CScriptBind_GameRules::RevivePlayerInVehicle(EntityId playerId, EntityId vehicleId, int seatId, int teamId, bool clearInventory)
{
	CGameRules *pGameRules = GetGameRules();

	if (!pGameRules)
		return;

	CActor *pActor = GetActor(playerId);

	if (pActor)
		pGameRules->RevivePlayerInVehicle(pActor, vehicleId, seatId, teamId, clearInventory);
}

//------------------------------------------------------------------------
void CScriptBind_GameRules::RenamePlayer(EntityId playerId, mono::string name)
{
	CGameRules *pGameRules=GetGameRules();

	if (!pGameRules)
		return;

	CActor *pActor = GetActor(playerId);

	if (pActor)
		pGameRules->RenamePlayer(pActor, *name);
}

//------------------------------------------------------------------------
void CScriptBind_GameRules::KillPlayer(EntityId playerId, bool dropItem, bool ragdoll, EntityId shooterId, EntityId weaponId, float damage, int material, int hit_type, Vec3 impulse)
{
	CGameRules *pGameRules=GetGameRules();

	if (!pGameRules)
		return;

	CActor *pActor = GetActor(playerId);

	if (pActor)
		pGameRules->KillPlayer(pActor, dropItem, ragdoll, shooterId, weaponId, damage, material, hit_type, impulse);

	return;
}

//------------------------------------------------------------------------
void CScriptBind_GameRules::MovePlayer(EntityId playerId, Vec3 pos, Vec3 angles)
{
	CGameRules *pGameRules=GetGameRules();

	if (!pGameRules)
		return;

	CActor *pActor = GetActor(playerId);

	if (pActor)
		pGameRules->MovePlayer(pActor, pos, Ang3(angles));
}

//------------------------------------------------------------------------
EntityId CScriptBind_GameRules::GetPlayerByChannelId(int channelId)
{
	CGameRules *pGameRules=GetGameRules();

	if (!pGameRules)
		return -1;

	CActor *pActor = pGameRules->GetActorByChannelId(channelId);
	if (pActor)
		return pActor->GetEntityId();

	return -1;
}

//------------------------------------------------------------------------
int CScriptBind_GameRules::GetChannelId(EntityId playerId)
{
	CGameRules *pGameRules=GetGameRules();
	if (!pGameRules)
		return -1;

	return pGameRules->GetChannelId(playerId);
}

//------------------------------------------------------------------------
int CScriptBind_GameRules::GetPlayerCount(bool inGame)
{
	CGameRules *pGameRules=GetGameRules();

	if (!pGameRules)
		return 0;

	return pGameRules->GetPlayerCount(inGame);
}

//------------------------------------------------------------------------
int CScriptBind_GameRules::GetSpectatorCount(bool inGame)
{
	CGameRules *pGameRules=GetGameRules();

	if (!pGameRules)
		return 0;

	return pGameRules->GetSpectatorCount(inGame);
}

//------------------------------------------------------------------------
mono::array CScriptBind_GameRules::GetPlayers()
{
	CGameRules *pGameRules=GetGameRules();

	if (!pGameRules)
		return NULL;

	CGameRules::TPlayers players;
	pGameRules->GetPlayers(players);

	IMonoArray *pArray = gEnv->pMonoScriptSystem->GetConverter()->CreateArray(players.size());
	for(CGameRules::TPlayers::iterator it = players.begin(); it != players.end(); ++it)
		pArray->Insert(*it);

	return *pArray;
}

//------------------------------------------------------------------------
bool CScriptBind_GameRules::IsPlayerInGame(EntityId playerId)
{
	CGameRules *pGameRules=GetGameRules();

	if(!pGameRules)
		return false;

	return pGameRules->IsPlayerInGame(playerId);
}

//------------------------------------------------------------------------
bool CScriptBind_GameRules::IsProjectile(EntityId entityId)
{
	CGameRules *pGameRules=GetGameRules();

	return pGameRules->IsProjectile(entityId);
}

//------------------------------------------------------------------------
bool CScriptBind_GameRules::IsSameTeam(EntityId entityId0, EntityId entityId1)
{
	CGameRules *pGameRules=GetGameRules();

	int t0=pGameRules->GetTeam(entityId0);
	int t1=pGameRules->GetTeam(entityId1);

	return t0==t1;
}

//------------------------------------------------------------------------
bool CScriptBind_GameRules::IsNeutral(EntityId entityId)
{
	CGameRules *pGameRules=GetGameRules();

	return pGameRules->GetTeam(entityId)==0;
}


//------------------------------------------------------------------------
void CScriptBind_GameRules::AddSpawnLocation(EntityId entityId)
{
	CGameRules *pGameRules=GetGameRules();

	if (pGameRules)
		pGameRules->AddSpawnLocation(entityId);
}

//------------------------------------------------------------------------
void CScriptBind_GameRules::RemoveSpawnLocation(EntityId id)
{
	CGameRules *pGameRules=GetGameRules();

	if (pGameRules)
		pGameRules->RemoveSpawnLocation(id);
}

//------------------------------------------------------------------------
int CScriptBind_GameRules::GetSpawnLocationCount()
{
	CGameRules *pGameRules=GetGameRules();

	if (pGameRules)
		return pGameRules->GetSpawnLocationCount();

	return 0;
}

//------------------------------------------------------------------------
EntityId CScriptBind_GameRules::GetSpawnLocationByIdx(int idx)
{
	CGameRules *pGameRules=GetGameRules();

	if (pGameRules)
	{
		EntityId id = pGameRules->GetSpawnLocation(idx);
		if (id)
			return id;
	}
	
	return -1;
}

//------------------------------------------------------------------------
mono::array CScriptBind_GameRules::GetSpawnLocations()
{
	CGameRules *pGameRules=GetGameRules();

	if (!pGameRules)
		return NULL;

	int spawnCount = pGameRules->GetSpawnLocationCount();
	IMonoArray *pArray = gEnv->pMonoScriptSystem->GetConverter()->CreateArray(spawnCount);
	for(int i = 0; i < spawnCount; i++)
		pArray->Insert(pGameRules->GetSpawnLocation(i));

	return *pArray;
}

//------------------------------------------------------------------------
EntityId CScriptBind_GameRules::GetSpawnLocation(EntityId playerId, bool ignoreTeam, bool includeNeutral, EntityId groupId, float minDistanceToDeath, float zOffset, Vec3 deathPos)
{
	CGameRules *pGameRules=GetGameRules();

	if (pGameRules)
	{
		EntityId id=pGameRules->GetSpawnLocation(playerId, ignoreTeam, includeNeutral, groupId, minDistanceToDeath, deathPos, &zOffset);
		if (id)
			return id;
	}

	return -1;
}

//------------------------------------------------------------------------
EntityId CScriptBind_GameRules::GetFirstSpawnLocation(int teamId)
{
	CGameRules *pGameRules=GetGameRules();

	if (pGameRules)
	{
		EntityId id=pGameRules->GetFirstSpawnLocation(teamId);
		if (id)
			return id;
	}

	return -1;
}

//------------------------------------------------------------------------
void CScriptBind_GameRules::AddSpawnGroup(EntityId groupId)
{
	CGameRules *pGameRules=GetGameRules();

	if (pGameRules)
		pGameRules->AddSpawnGroup(groupId);
}

//------------------------------------------------------------------------
void CScriptBind_GameRules::AddSpawnLocationToSpawnGroup(EntityId groupId, EntityId location)
{
	CGameRules *pGameRules=GetGameRules();

	if (pGameRules)
		pGameRules->AddSpawnLocationToSpawnGroup(groupId, location);
}

//------------------------------------------------------------------------
void CScriptBind_GameRules::RemoveSpawnLocationFromSpawnGroup(EntityId groupId, EntityId location)
{
	CGameRules *pGameRules=GetGameRules();

	if (pGameRules)
		pGameRules->RemoveSpawnLocationFromSpawnGroup(groupId, location);
}

//------------------------------------------------------------------------
void CScriptBind_GameRules::RemoveSpawnGroup(EntityId groupId)
{
	CGameRules *pGameRules=GetGameRules();

	if (pGameRules)
		pGameRules->RemoveSpawnGroup(groupId);
}

//------------------------------------------------------------------------
EntityId CScriptBind_GameRules::GetSpawnLocationGroup(EntityId spawnId)
{
	CGameRules *pGameRules=GetGameRules();

	if (pGameRules)
	{
		EntityId groupId=pGameRules->GetSpawnLocationGroup(spawnId);
		if (groupId)
			return groupId;
	}

	return -1;
}

//------------------------------------------------------------------------
mono::array CScriptBind_GameRules::GetSpawnGroups(int teamId)
{
	CGameRules *pGameRules=GetGameRules();

	if (!pGameRules)
		return NULL;

	int spawnGroupCount = pGameRules->GetSpawnGroupCount();
	IMonoArray *pArray = gEnv->pMonoScriptSystem->GetConverter()->CreateArray(spawnGroupCount);
	for(int i = 0; i < spawnGroupCount; i++)
		pArray->Insert(pGameRules->GetSpawnGroup(i));

	return *pArray;
}

//------------------------------------------------------------------------
bool CScriptBind_GameRules::IsSpawnGroup(EntityId entityId)
{
	CGameRules *pGameRules=GetGameRules();

	if (!pGameRules)
		return false;

	return pGameRules->IsSpawnGroup(entityId);
}

//------------------------------------------------------------------------
EntityId CScriptBind_GameRules::GetTeamDefaultSpawnGroup(int teamId)
{
	CGameRules *pGameRules=GetGameRules();
	if (pGameRules)
	{
		EntityId id=pGameRules->GetTeamDefaultSpawnGroup(teamId);
		if (id)
			return id;
	}

	return -1;
}

//------------------------------------------------------------------------
void CScriptBind_GameRules::SetTeamDefaultSpawnGroup(int teamId, EntityId groupId)
{
	CGameRules *pGameRules=GetGameRules();
	if (pGameRules)
		pGameRules->SetTeamDefaultSpawnGroup(teamId, groupId);
}

//------------------------------------------------------------------------
void CScriptBind_GameRules::SetPlayerSpawnGroup(EntityId playerId, EntityId groupId)
{
	CGameRules *pGameRules=GetGameRules();
	if (pGameRules)
		pGameRules->SetPlayerSpawnGroup(playerId, groupId);
}

//------------------------------------------------------------------------
void CScriptBind_GameRules::AddSpectatorLocation(EntityId location)
{
	CGameRules *pGameRules=GetGameRules();

	if (pGameRules)
		pGameRules->AddSpectatorLocation(location);
}

//------------------------------------------------------------------------
void CScriptBind_GameRules::RemoveSpectatorLocation(EntityId id)
{
	CGameRules *pGameRules=GetGameRules();

	if (pGameRules)
		pGameRules->RemoveSpectatorLocation(id);
}

//------------------------------------------------------------------------
int CScriptBind_GameRules::GetSpectatorLocationCount()
{
	CGameRules *pGameRules=GetGameRules();

	if (pGameRules)
		return pGameRules->GetSpectatorLocationCount();

	return 0;
}

//------------------------------------------------------------------------
EntityId CScriptBind_GameRules::GetSpectatorLocation(int idx)
{
	CGameRules *pGameRules=GetGameRules();

	if (pGameRules)
		return pGameRules->GetSpectatorLocation(idx);

	return 0;
}

//------------------------------------------------------------------------
mono::array CScriptBind_GameRules::GetSpectatorLocations()
{
	CGameRules *pGameRules=GetGameRules();

	if (!pGameRules)
		return NULL;

	int spectatorLocationCount = pGameRules->GetSpectatorLocationCount();
	IMonoArray *pArray = gEnv->pMonoScriptSystem->GetConverter()->CreateArray(spectatorLocationCount);
	for(int i = 0; i < spectatorLocationCount; i++)
		pArray->Insert(pGameRules->GetSpectatorLocation(i));

	return *pArray;
}

//------------------------------------------------------------------------
EntityId CScriptBind_GameRules::GetRandomSpectatorLocation()
{
	CGameRules *pGameRules=GetGameRules();

	if (pGameRules)
	{
		if (EntityId locationId=pGameRules->GetRandomSpectatorLocation())
			return locationId;
	}

	return -1;
}

//------------------------------------------------------------------------
EntityId CScriptBind_GameRules::GetInterestingSpectatorLocation()
{
	CGameRules *pGameRules=GetGameRules();

	if (pGameRules)
	{
		if (EntityId locationId=pGameRules->GetInterestingSpectatorLocation())
			return locationId;
	}

	return -1;
}

//------------------------------------------------------------------------
EntityId CScriptBind_GameRules::GetNextSpectatorTarget(EntityId playerId, int change)
{
	if(change >= 1) change = 1;
	if(change <= 0) change = -1;
	CGameRules* pGameRules = GetGameRules();
	if(pGameRules)
	{
		CPlayer* pPlayer = (CPlayer*)gEnv->pGame->GetIGameFramework()->GetIActorSystem()->GetActor(playerId);
		if(pPlayer)
		{
			// get list of possible players (team mates or all players)
			CGameRules::TPlayers players;
			int team = pGameRules->GetTeam(playerId);
			if(g_pGameCVars->g_spectate_TeamOnly == 0 || pGameRules->GetTeamCount() == 0 || team == 0)
			{
				pGameRules->GetPlayers(players);
			}
			else
			{
				pGameRules->GetTeamPlayers(team, players);
			}
			int numPlayers = players.size();

			// work out which one we are currently watching
			int index = 0;
			for(; index < players.size(); ++index)
				if(players[index] == pPlayer->GetSpectatorTarget())
					break;

			// loop through the players to find a valid one.
			bool found = false;
			if(numPlayers > 0)
			{
				int newTargetIndex = index;
				int numAttempts = numPlayers;
				do
				{
					newTargetIndex += change;
					--numAttempts;

					// wrap around
					if(newTargetIndex < 0)
						newTargetIndex = numPlayers-1;
					if(newTargetIndex >= numPlayers)
						newTargetIndex = 0;

					// skip ourself
					if(players[newTargetIndex] == playerId)
						continue;

					// skip dead players
					IActor* pActor = gEnv->pGame->GetIGameFramework()->GetIActorSystem()->GetActor(players[newTargetIndex]);
					if(!pActor)
						continue;				

					// skip spectating players
					if(((CPlayer*)pActor)->GetSpectatorMode() != CActor::eASM_None)
						continue;
					
					// otherwise this one will do.
					found = true;
				} while(!found && numAttempts > 0);

				if(found)
					return players[newTargetIndex];
			}
		}
	}
	return 0;
}

//------------------------------------------------------------------------
void CScriptBind_GameRules::ChangeSpectatorMode(EntityId playerId, int mode, EntityId targetId)
{
	CGameRules *pGameRules = GetGameRules();
	CActor* pActor = (CActor*)g_pGame->GetIGameFramework()->GetIActorSystem()->GetActor(playerId);

	if(pGameRules && pActor)
		pGameRules->ChangeSpectatorMode(pActor, mode, targetId, false);
}

//------------------------------------------------------------------------
bool CScriptBind_GameRules::CanChangeSpectatorMode(EntityId playerId)
{
	IActor* pActor = g_pGame->GetIGameFramework()->GetClientActor();
	
	return true;
}

//------------------------------------------------------------------------
void CScriptBind_GameRules::AddMinimapEntity(EntityId entityId, int type, float lifetime)
{
	CGameRules *pGameRules=GetGameRules();

	if (!pGameRules)
		return;

	pGameRules->AddMinimapEntity(entityId, type, lifetime);
}


//------------------------------------------------------------------------
void CScriptBind_GameRules::RemoveMinimapEntity(EntityId entityId)
{
	CGameRules *pGameRules=GetGameRules();

	if (!pGameRules)
		return;

	pGameRules->RemoveMinimapEntity(entityId);
}


//------------------------------------------------------------------------
void CScriptBind_GameRules::ResetMinimap()
{
	CGameRules *pGameRules=GetGameRules();

	if (!pGameRules)
		return;

	pGameRules->ResetMinimap();
}


//------------------------------------------------------------------------
float CScriptBind_GameRules::GetPing(int channelId)
{
	CGameRules *pGameRules=GetGameRules();

	if (!pGameRules)
		return 0;

	INetChannel *pNetChannel = g_pGame->GetIGameFramework()->GetNetChannel(channelId);
	if (pNetChannel)
		return pNetChannel->GetPing(true);

	return 0;
}

//------------------------------------------------------------------------
void CScriptBind_GameRules::ResetEntities()
{
	CGameRules *pGameRules=GetGameRules();

	if (pGameRules)
		pGameRules->ResetEntities();
}

//------------------------------------------------------------------------
void CScriptBind_GameRules::ServerExplosion(EntityId shooterId, EntityId weaponId, float dmg, Vec3 pos, Vec3 dir, float radius, float angle, float pressure, float holesize,
	mono::string _effect, float effectScale, int type, float _minRadius, float _minPhysRadius, float _physRadius)
{
	CGameRules *pGameRules=GetGameRules();

	if (!gEnv->bServer)
		return;

	const char *effect = "";
	if(_effect!=0)
		effect = *_effect;

	float minRadius = radius/2.0f;
	if(_minRadius!=-1.0f)
		minRadius = _minRadius;
	float minPhysRadius = radius/2.0f;
	if(_minPhysRadius!=-1.0f)
		minPhysRadius = _minPhysRadius;
	float physRadius = radius;
	if(_physRadius!=-1.0f)
		physRadius = _physRadius;

	ExplosionInfo info(shooterId, weaponId, 0, dmg, pos, dir.GetNormalized(), minRadius, radius, minPhysRadius, physRadius, angle, pressure, holesize, 0);
	info.SetEffect(effect, effectScale, 0.0f);
	info.type = type;

	pGameRules->ServerExplosion(info);
}

//------------------------------------------------------------------------
void CScriptBind_GameRules::ServerHit(EntityId targetId, EntityId shooterId, EntityId weaponId, float dmg, float radius, int materialId, int partId, int typeId, Vec3 pos, Vec3 dir, Vec3 normal)
{
	CGameRules *pGameRules=GetGameRules();

	if (!gEnv->bServer)
		return;

	HitInfo info(shooterId, targetId, weaponId, dmg, radius, materialId, partId, typeId, pos, dir, normal);

	pGameRules->ServerHit(info);
}

//------------------------------------------------------------------------
int CScriptBind_GameRules::CreateTeam(mono::string name)
{
	CGameRules *pGameRules=GetGameRules();

	if (!pGameRules)
		return 0;

	return pGameRules->CreateTeam(*name);
}

//------------------------------------------------------------------------
void CScriptBind_GameRules::RemoveTeam(int teamId)
{
	CGameRules *pGameRules=GetGameRules();

	if (!pGameRules)
		return;

	pGameRules->RemoveTeam(teamId);
}

//------------------------------------------------------------------------
mono::string CScriptBind_GameRules::GetTeamName(int teamId)
{
	CGameRules *pGameRules=GetGameRules();

	if (!pGameRules)
		return (mono::string)ToMonoString("");

	return (mono::string)ToMonoString(pGameRules->GetTeamName(teamId));
}

//------------------------------------------------------------------------
int CScriptBind_GameRules::GetTeamId(mono::string teamName)
{
	CGameRules *pGameRules=GetGameRules();

	if (!pGameRules)
		return 0;

	return pGameRules->GetTeamId(*teamName);
}

//------------------------------------------------------------------------
int CScriptBind_GameRules::GetTeamCount()
{
	CGameRules *pGameRules=GetGameRules();

	if (!pGameRules)
		return 0;

	return pGameRules->GetTeamCount();
}

//------------------------------------------------------------------------
int CScriptBind_GameRules::GetTeamPlayerCount(int teamId, bool inGame)
{
	CGameRules *pGameRules=GetGameRules();

	if (!pGameRules)
		return 0;

	return pGameRules->GetTeamPlayerCount(teamId, inGame);
}

//------------------------------------------------------------------------
int CScriptBind_GameRules::GetTeamChannelCount(int teamId, bool inGame)
{
	CGameRules *pGameRules=GetGameRules();

	if (!pGameRules)
		return 0;

	return pGameRules->GetTeamChannelCount(teamId, inGame);
}

//------------------------------------------------------------------------
mono::array CScriptBind_GameRules::GetTeamPlayers(int teamId)
{
	CGameRules *pGameRules=GetGameRules();

	if (!pGameRules)
		return NULL;

	CGameRules::TPlayers players;
	pGameRules->GetPlayers(players);

	IMonoArray *pArray = gEnv->pMonoScriptSystem->GetConverter()->CreateArray(players.size());
	for(CGameRules::TPlayers::iterator it = players.begin(); it != players.end(); ++it)
	{
		if(pGameRules->GetTeam(*it)==teamId)
			pArray->Insert(*it);
	}

	return *pArray;
}

//------------------------------------------------------------------------
void CScriptBind_GameRules::SetTeam(int teamId, EntityId playerId)
{
	CGameRules *pGameRules=GetGameRules();

	if (!pGameRules)
		return;

	pGameRules->SetTeam(teamId, playerId);
}

//------------------------------------------------------------------------
int CScriptBind_GameRules::GetTeam(EntityId playerId)
{
	CGameRules *pGameRules=GetGameRules();

	if (!pGameRules)
		return 0;

	return pGameRules->GetTeam(playerId);
}

//------------------------------------------------------------------------
int CScriptBind_GameRules::GetChannelTeam(int channelId)
{
	CGameRules *pGameRules=GetGameRules();

	if (!pGameRules)
		return 0;

	return pGameRules->GetChannelTeam(channelId);
}

//------------------------------------------------------------------------
void CScriptBind_GameRules::AddObjective(int teamId, mono::string objective, int status, EntityId entityId)
{
	CGameRules *pGameRules=GetGameRules();

	if (!pGameRules)
		return;

	pGameRules->AddObjective(teamId, *objective, (CGameRules::EMissionObjectiveState) status, entityId);
}

//------------------------------------------------------------------------
void CScriptBind_GameRules::SetObjectiveStatus(int teamId, mono::string objective, int status)
{
	CGameRules *pGameRules=GetGameRules();

	if (!pGameRules)
		return;

	pGameRules->SetObjectiveStatus(teamId, *objective, (CGameRules::EMissionObjectiveState) status);
}

//------------------------------------------------------------------------
void CScriptBind_GameRules::SetObjectiveEntity(int teamId, mono::string objective, EntityId entityId)
{
	CGameRules *pGameRules=GetGameRules();

	if (!pGameRules)
		return;

	pGameRules->SetObjectiveEntity(teamId, *objective, entityId);
}

//------------------------------------------------------------------------
void CScriptBind_GameRules::RemoveObjective(int teamId, mono::string objective)
{
	CGameRules *pGameRules=GetGameRules();

	if (!pGameRules)
		return;

	pGameRules->RemoveObjective(teamId, *objective);
}

//------------------------------------------------------------------------
void CScriptBind_GameRules::ResetObjectives()
{
	CGameRules *pGameRules=GetGameRules();

	if (!pGameRules)
		return;

	pGameRules->ResetObjectives();
}

//------------------------------------------------------------------------
void CScriptBind_GameRules::ForbiddenAreaWarning(bool active, int timer, EntityId targetId)
{
	CGameRules *pGameRules=GetGameRules();

	if (!pGameRules)
		return;

	pGameRules->ForbiddenAreaWarning(active, timer, targetId);
}

//------------------------------------------------------------------------
void CScriptBind_GameRules::ResetGameTime()
{
	CGameRules *pGameRules=GetGameRules();

	if (!pGameRules)
		return;

	pGameRules->ResetGameTime();
}

//------------------------------------------------------------------------
float CScriptBind_GameRules::GetRemainingGameTime()
{
	CGameRules *pGameRules=GetGameRules();

	if (!pGameRules)
		return 0.0f;

	return pGameRules->GetRemainingGameTime();
}

//------------------------------------------------------------------------
bool CScriptBind_GameRules::IsTimeLimited()
{
	CGameRules *pGameRules=GetGameRules();

	if (!pGameRules)
		return false;

	return pGameRules->IsTimeLimited();
}

//------------------------------------------------------------------------
void CScriptBind_GameRules::ResetRoundTime()
{
	CGameRules *pGameRules=GetGameRules();

	if (!pGameRules)
		return;

	pGameRules->ResetRoundTime();
}

//------------------------------------------------------------------------
float CScriptBind_GameRules::GetRemainingRoundTime()
{
	CGameRules *pGameRules=GetGameRules();

	if (!pGameRules)
		return 0;

	return pGameRules->GetRemainingRoundTime();
}

//------------------------------------------------------------------------
bool CScriptBind_GameRules::IsRoundTimeLimited()
{
	CGameRules *pGameRules=GetGameRules();

	if (!pGameRules || !pGameRules->IsRoundTimeLimited())
		return false;

	return true;
}

//------------------------------------------------------------------------
void CScriptBind_GameRules::ResetPreRoundTime()
{
	CGameRules *pGameRules=GetGameRules();

	if (!pGameRules)
		return;

	pGameRules->ResetPreRoundTime();
}

//------------------------------------------------------------------------
float CScriptBind_GameRules::GetRemainingPreRoundTime()
{
	CGameRules *pGameRules=GetGameRules();

	if (!pGameRules)
		return 0.0f;

	return pGameRules->GetRemainingPreRoundTime();
}

//------------------------------------------------------------------------
void CScriptBind_GameRules::ResetReviveCycleTime()
{
	CGameRules *pGameRules=GetGameRules();

	if (!pGameRules)
		return;

	pGameRules->ResetReviveCycleTime();
}

//------------------------------------------------------------------------
float CScriptBind_GameRules::GetRemainingReviveCycleTime()
{
	CGameRules *pGameRules=GetGameRules();

	if (!pGameRules)
		return 0.0f;

	return pGameRules->GetRemainingReviveCycleTime();
}

//------------------------------------------------------------------------
void CScriptBind_GameRules::ResetGameStartTimer(float time)
{
	CGameRules *pGameRules=GetGameRules();

	if (!pGameRules)
		return;

	pGameRules->ResetGameStartTimer(time);

	return;
}

//------------------------------------------------------------------------
float CScriptBind_GameRules::GetRemainingStartTimer()
{
	CGameRules *pGameRules=GetGameRules();

	if (!pGameRules)
		return 0.0f;

	return pGameRules->GetRemainingStartTimer();
}

//------------------------------------------------------------------------
void CScriptBind_GameRules::EndGame()
{
	CGameRules *pGameRules=GetGameRules();

	if (pGameRules)
		pGameRules->OnEndGame();
}

//------------------------------------------------------------------------
void CScriptBind_GameRules::NextLevel()
{
	CGameRules *pGameRules=GetGameRules();

	if (pGameRules)
		pGameRules->NextLevel();
}

//------------------------------------------------------------------------
int CScriptBind_GameRules::GetHitMaterialId(mono::string materialName)
{
	CGameRules *pGameRules=GetGameRules();

	return pGameRules->GetHitMaterialId(*materialName);
}

//------------------------------------------------------------------------
mono::string CScriptBind_GameRules::GetHitMaterialName(int materialId)
{
	CGameRules *pGameRules=GetGameRules();

	if (ISurfaceType *pSurfaceType=pGameRules->GetHitMaterial(materialId))
		return (mono::string )ToMonoString(pSurfaceType->GetName());

	return 0;
}

//------------------------------------------------------------------------
void CScriptBind_GameRules::ResetHitMaterials()
{
	CGameRules *pGameRules=GetGameRules();
	pGameRules->ResetHitMaterials();
}

//------------------------------------------------------------------------
int CScriptBind_GameRules::RegisterHitType(mono::string type)
{
	CGameRules *pGameRules=GetGameRules();

	return pGameRules->RegisterHitType(*type);
}

//------------------------------------------------------------------------
int CScriptBind_GameRules::GetHitTypeId(mono::string type)
{
	CGameRules *pGameRules=GetGameRules();

	return pGameRules->GetHitTypeId(*type);
}

//------------------------------------------------------------------------
mono::string CScriptBind_GameRules::GetHitType(int id)
{
	CGameRules *pGameRules=GetGameRules();

	return (mono::string )ToMonoString(pGameRules->GetHitType(id));
}

//------------------------------------------------------------------------
void CScriptBind_GameRules::ResetHitTypes()
{
	CGameRules *pGameRules=GetGameRules();
	pGameRules->ResetHitTypes();
}

//------------------------------------------------------------------------
void CScriptBind_GameRules::ForceScoreboard(bool force)
{
	CGameRules *pGameRules=GetGameRules();
	pGameRules->ForceScoreboard(force);
}

//------------------------------------------------------------------------
void CScriptBind_GameRules::FreezeInput(bool freeze)
{
	CGameRules *pGameRules=GetGameRules();
	pGameRules->FreezeInput(freeze);
}

//------------------------------------------------------------------------
void CScriptBind_GameRules::ScheduleEntityRespawn(EntityId entityId, bool unique, float timer)
{
	CGameRules *pGameRules=GetGameRules();
	pGameRules->ScheduleEntityRespawn(entityId, unique, timer);
}

//------------------------------------------------------------------------
void CScriptBind_GameRules::AbortEntityRespawn(EntityId entityId, bool destroyData)
{
	CGameRules *pGameRules=GetGameRules();
	pGameRules->AbortEntityRespawn(entityId, destroyData);
}

//------------------------------------------------------------------------
void CScriptBind_GameRules::ScheduleEntityRemoval(EntityId entityId, float timer, bool visibility)
{
	CGameRules *pGameRules=GetGameRules();
	pGameRules->ScheduleEntityRemoval(entityId, timer, visibility);
}

//------------------------------------------------------------------------
void CScriptBind_GameRules::AbortEntityRemoval(EntityId entityId)
{
	CGameRules *pGameRules=GetGameRules();
	pGameRules->AbortEntityRemoval(entityId);
}

//------------------------------------------------------------------------
bool CScriptBind_GameRules::IsDemoMode()
{
	return IsDemoPlayback();
}

//------------------------------------------------------------------------
float CScriptBind_GameRules::GetTimeLimit()
{
	return g_pGameCVars->g_timelimit;
}

//------------------------------------------------------------------------
float CScriptBind_GameRules::GetRoundTime()
{
	return g_pGameCVars->g_roundtime;
}

//------------------------------------------------------------------------
int CScriptBind_GameRules::GetPreRoundTime()
{
	return g_pGameCVars->g_preroundtime;
}

//------------------------------------------------------------------------
int CScriptBind_GameRules::GetRoundLimit()
{
	return g_pGameCVars->g_roundlimit;
}

//------------------------------------------------------------------------
int CScriptBind_GameRules::GetFragLimit()
{
	return g_pGameCVars->g_fraglimit;
}

//------------------------------------------------------------------------
int CScriptBind_GameRules::GetFragLead()
{
  return g_pGameCVars->g_fraglead;
}

//------------------------------------------------------------------------
float CScriptBind_GameRules::GetFriendlyFireRatio()
{
  return g_pGameCVars->g_friendlyfireratio;
}

//------------------------------------------------------------------------
int CScriptBind_GameRules::GetReviveTime()
{
  return g_pGameCVars->g_revivetime;
}

int CScriptBind_GameRules::GetMinPlayerLimit()
{
	return g_pGameCVars->g_minplayerlimit;
}

//------------------------------------------------------------------------
int CScriptBind_GameRules::GetMinTeamLimit()
{
	return g_pGameCVars->g_minteamlimit;
}

//------------------------------------------------------------------------
int CScriptBind_GameRules::GetTeamLock()
{
	return g_pGameCVars->g_teamlock;
}

//------------------------------------------------------------------------
bool CScriptBind_GameRules::IsFrozen(EntityId entityId)
{
	return GetGameRules()->IsFrozen(entityId);
}

//------------------------------------------------------------------------
void CScriptBind_GameRules::FreezeEntity(EntityId entityId, bool freeze, bool vapor, bool force)
{
	GetGameRules()->FreezeEntity(entityId, freeze, vapor, force);
}

//------------------------------------------------------------------------
void CScriptBind_GameRules::ShatterEntity(EntityId entityId, Vec3 pos, Vec3 impulse)
{
	GetGameRules()->ShatterEntity(entityId, pos, impulse);
}

#ifndef _RELEASE
//------------------------------------------------------------------------
int CScriptBind_GameRules::DebugCollisionDamage()
{
  return g_pGameCVars->g_debugCollisionDamage;
}

//------------------------------------------------------------------------
int CScriptBind_GameRules::DebugHits()
{
	return g_pGameCVars->g_debugHits;
}
#endif

//------------------------------------------------------------------------
void CScriptBind_GameRules::SendHitIndicator(EntityId shooterId)
{
	if(!gEnv->bServer)
		return;

	EntityId id = EntityId(shooterId);
	if(!id)
		return;

	CGameRules *pGameRules = GetGameRules();

	CActor* pActor = pGameRules->GetActorByEntityId(id);
	if (!pActor || !pActor->IsPlayer())
		return;

	pGameRules->GetGameObject()->InvokeRMI(CGameRules::ClHitIndicator(), CGameRules::NoParams(), eRMI_ToClientChannel, pGameRules->GetChannelId(id));
}

//------------------------------------------------------------------------
void CScriptBind_GameRules::SendDamageIndicator(EntityId targetId, EntityId shooterId, EntityId weaponId)
{
	if(!gEnv->bServer)
		return;

	CGameRules *pGameRules = GetGameRules();

	EntityId tId = EntityId(targetId);
	EntityId sId= EntityId(shooterId);
	EntityId wId= EntityId(weaponId);

	if (!tId)
		return;

	CActor* pActor = pGameRules->GetActorByEntityId(tId);
	if (!pActor || !pActor->IsPlayer())
		return;

	pGameRules->GetGameObject()->InvokeRMIWithDependentObject(CGameRules::ClDamageIndicator(), CGameRules::DamageIndicatorParams(sId, wId), eRMI_ToClientChannel, sId, pGameRules->GetChannelId(tId));
}

//-------------------------------------------------------------------------
void CScriptBind_GameRules::GameOver(int localWinner)
{
	CGameRules* pGameRules = g_pGame->GetGameRules();
	if (pGameRules)
		pGameRules->GameOver(localWinner);
}

//-------------------------------------------------------------------------
void CScriptBind_GameRules::EnteredGame()
{
	CGameRules* pGameRules = g_pGame->GetGameRules();
	if (pGameRules)
		pGameRules->EnteredGame();
}

//--------------------------------------------------------------------------
void CScriptBind_GameRules::EndGameNear(EntityId entityId)
{
	CGameRules* pGameRules = g_pGame->GetGameRules();
	if(pGameRules)
		pGameRules->EndGameNear(entityId);
}

void CScriptBind_GameRules::SPNotifyPlayerKill(EntityId targetId, EntityId weaponId, bool bHeadShot)
{
	CGameRules* pGameRules = g_pGame->GetGameRules();
	if(pGameRules)
		pGameRules->SPNotifyPlayerKill(targetId, weaponId, bHeadShot);
}

//-----------------------------------------------------------------------------
void CScriptBind_GameRules::ProcessEMPEffect(EntityId targetId, float timeScale)
{
	if(timeScale>0.0f)
	{
		CActor* pActor = static_cast<CActor*>(g_pGame->GetIGameFramework()->GetIActorSystem()->GetActor(EntityId(targetId)));
		if (pActor && (pActor->GetSpectatorMode() == 0) && (pActor->GetActorClass() == CPlayer::GetActorClassType()))
		{
			const float baseTime = 15.0f;
			float time = max(3.0f, baseTime*timeScale);
			CPlayer* pPlayer = static_cast<CPlayer*>(pActor);
			pPlayer->GetGameObject()->InvokeRMI(CPlayer::ClEMP(), CPlayer::EMPParams(time), eRMI_ToClientChannel, pPlayer->GetChannelId());			
		}
	}
}

//-----------------------------------------------------------------------------
bool CScriptBind_GameRules::PerformDeadHit()
{
#ifdef CRAPDOLLS
	return false;
#else
	return true;
#endif // CRAPDOLLS
}

//-----------------------------------------------------------------------------
void CScriptBind_GameRules::RegisterGameMode(mono::string gamemode)
{
	// gEnv->pGameFramework is set too late, so we'll have to set it earlier in CGameStartup::InitFramework. (gEnv->pGameFramework = m_pFramework after the ModuleInitISystem call)
	if(IGameRulesSystem *pGameRulesSystem = gEnv->pGameFramework ? gEnv->pGameFramework->GetIGameRulesSystem() : NULL)
	{
		if(!pGameRulesSystem->HaveGameRules(*gamemode))
			pGameRulesSystem->RegisterGameRules(*gamemode, "GameRules");
	}
}

//-----------------------------------------------------------------------------
void CScriptBind_GameRules::AddGameModeAlias(mono::string gamemode, mono::string alias)
{
	gEnv->pGameFramework->GetIGameRulesSystem()->AddGameRulesAlias(*gamemode, *alias);
}

//-----------------------------------------------------------------------------
void CScriptBind_GameRules::AddGameModeLevelLocation(mono::string gamemode, mono::string location)
{
	gEnv->pGameFramework->GetIGameRulesSystem()->AddGameRulesLevelLocation(*gamemode, *location);
}

//-----------------------------------------------------------------------------
void CScriptBind_GameRules::SetDefaultGameMode(mono::string gamemode)
{
	gEnv->pConsole->GetCVar("sv_gamerulesdefault")->Set(*gamemode);
}