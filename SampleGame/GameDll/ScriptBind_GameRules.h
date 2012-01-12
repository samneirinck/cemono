/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2004.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Script Binding for GameRules

-------------------------------------------------------------------------
History:
- 23:2:2006   18:30 : Created by Márcio Martins

*************************************************************************/
#ifndef __SCRIPTBIND_GAMERULES_H__
#define __SCRIPTBIND_GAMERULES_H__

#if _MSC_VER > 1000
# pragma once
#endif

#include <IMonoScriptBind.h>
#include <MonoCommon.h>

class CGameRules;
class CActor;
struct IGameFramework;
struct ISystem;

class CScriptBind_GameRules : public IMonoScriptBind
{
public:
	CScriptBind_GameRules(ISystem *pSystem, IGameFramework *pGameFramework);
	virtual ~CScriptBind_GameRules();

	// <title SpawnPlayer>
	// Syntax: GameRules.SpawnPlayer( int channelId, mono::string name, mono::string className, Vec3 pos, Vec3 angles )
	// Arguments:
	//		channelId	- Channel identifier.
	//		name		- Player name.
	//		className	- Name of the player class.
	//		pos			- Player position.
	//		angles		- Player angle.
	// Description:
	//		Spawns a player.
	static EntityId SpawnPlayer(int channelId, mono::string name, mono::string className, Vec3 pos, Vec3 angles);
	// <title ChangePlayerClass>
	// Syntax: GameRules.ChangePlayerClass( int channelId, mono::string className )
	// Arguments:
	//		channelId - Channel identifier.
	//		className - Name of the new class for the player.
	// Description:
	//		Changes the player class.
	//int ChangePlayerClass(int channelId, mono::string className);
	// <title RevivePlayer>
	// Syntax: GameRules.RevivePlayer( EntityId playerId, Vec3 pos, Vec3 angles, int teamId, bool clearInventory )
	// Arguments:
	//		playerId		- Player identifier.
	//		pos				- Player position.
	//		angles			- Player angle.
	//		teamId			- Team identifier.
	//		clearInventory	- True to clean the inventory, false otherwise. 
	// Description:
	//		Revives the player.
	static void RevivePlayer(EntityId playerId, Vec3 pos, Vec3 rot, int teamId, bool clearInventory);
	// <title RevivePlayerInVehicle>
	// Syntax: GameRules.RevivePlayerInVehicle( EntityId playerId, EntityId vehicleId, int seatId, int teamId, bool clearInventory )
	// Arguments:
	//		playerId		- Player identifier.
	//		vehicleId		- Vehicle identifier.
	//		seatId			- Seat identifier.
	//		teamId			- Team identifier.
	//		clearInventory	- True to clean the inventory, false otherwise.
	// Description:
	//		Revives a player inside a vehicle.
	static void RevivePlayerInVehicle(EntityId playerId, EntityId vehicleId, int seatId, int teamId, bool clearInventory);
	// <title RenamePlayer>
	// Syntax: GameRules.RenamePlayer( EntityId playerId, mono::string name )
	// Arguments:
	//		playerId - Player identifier.
	//		name	 - New name for the player.
	// Description:
	//		Renames the player.
	static void RenamePlayer(EntityId playerId, mono::string name);
	// <title KillPlayer>
	// Syntax: GameRules.KillPlayer( EntityId playerId, bool dropItem, bool ragdoll,
	//		EntityId shooterId, EntityId weaponId, float damage, int material, int hit_type, Vec3 impulse)
	// Arguments:
	//		playerId	- Player identifier.
	//		dropItem	- True to drop the item, false otherwise.
	//		ragdoll		- True to ragdollize, false otherwise.
	//		shooterId	- Shooter identifier.
	//		weaponId	- Weapon identifier.
	//		damage		- Damage amount.
	//		material	- Material identifier.
	//		hit_type	- Type of the hit.
	//		impulse		- Impulse vector due to the hit.
	// Description:
	//		Kills the player.
	static void KillPlayer(EntityId playerId, bool dropItem, bool ragdoll, EntityId shooterId, EntityId weaponId, float damage, int material, int hit_type, Vec3 impulse);
	// <title MovePlayer>
	// Syntax: GameRules.MovePlayer( EntityId playerId, Vec3 pos, Vec3 angles )
	// Arguments:
	//		playerId - Player identifier.
	//		pos		 - Position to be reached.
	//		angles	 - .
	// Description:
	//		Moves the player.
	static void MovePlayer(EntityId playerId, Vec3 pos, Vec3 angles);
	// <title GetPlayerByChannelId>
	// Syntax: GameRules.GetPlayerByChannelId( int channelId )
	// Arguments:
	//		channelId - Channel identifier.
	// Description: 
	//		Gets the player from the channel id.
	static EntityId GetPlayerByChannelId(int channelId);
	// <title GetChannelId>
	// Syntax: GameRules.GetChannelId( playerId )
	// Arguments:
	//		playerId - Player identifier.
	// Description:
	//		Gets the channel id where the specified player is.
	static int GetChannelId(EntityId playerId);
	// <title GetPlayerCount>
	// Syntax: GameRules.GetPlayerCount()
	// Description:
	//		Gets the number of the players.
	static int GetPlayerCount(bool inGame = false);
	// <title GetSpectatorCount>
	// Syntax: GameRules.GetSpectatorCount()
	// Description:
	//		Gets the number of the spectators.
	static int GetSpectatorCount(bool inGame = false);
	// <title GetPlayers>
	// Syntax: GameRules.GetPlayers()
	// Description:
	//		Gets the player.
	static mono::array GetPlayers();
	// <title IsPlayerInGame>
	// Syntax: GameRules.IsPlayerInGame(playerId)
	// Arguments:
	//		playerId - Player identifier.
	// Description:
	//		Checks if the specified player is in game.
	static bool IsPlayerInGame(EntityId playerId);
	// <title IsProjectile>
	// Syntax: GameRules.IsProjectile(entityId)
	// Arguments:
	//		entityId - Entity identifier.
	// Description:
	//		Checks if the specified entity is a projectile.
	static bool IsProjectile(EntityId entityId);
	// <title IsSameTeam>
	// Syntax: GameRules.IsSameTeam(entityId0,entityId1)
	// Arguments:
	//		entityId0 - First entity identifier.
	//		entityId1 - Second entity identifier.
	// Description:
	//		Checks if the two entity are in the same team.
	static bool IsSameTeam(EntityId entityId0, EntityId entityId1);
	// <title IsNeutral>
	// Syntax: GameRules.IsNeutral(entityId)
	// Arguments:
	//		entityId - Entity identifier.
	// Description:
	//		Checks if the entity is neutral.
	static bool IsNeutral(EntityId entityId);

	// <title AddSpawnLocation>
	// Syntax: GameRules.AddSpawnLocation(entityId)
	// Arguments:
	//		entityId - Entity identifier.
	// Description:
	//		Adds the spawn location for the specified entity.
	static void AddSpawnLocation(EntityId entityId);
	// <title RemoveSpawnLocation>
	// Syntax: GameRules.RemoveSpawnLocation(id)
	// Arguments:
	//		id - Identifier for the script.
	// Description:
	//		Removes the span location.
	static void RemoveSpawnLocation(EntityId id);
	// <title GetSpawnLocationCount>
	// Syntax: GameRules.GetSpawnLocationCount()
	// Description:
	//		Gets the number of the spawn location.
	static int GetSpawnLocationCount();
	// <title GetSpawnLocationByIdx>
	// Syntax: GameRules.GetSpawnLocationByIdx( int idx )
	// Arguments:
	//		idx - Spawn location identifier.
	// Description:
	//		Gets the spawn location from its identifier.
	static EntityId GetSpawnLocationByIdx(int idx);
	// <title GetSpawnLocation>
	// Syntax: GameRules.GetSpawnLocation( playerId, bool ignoreTeam, bool includeNeutral )
	// Arguments:
	//		playerId		- Player identifier.
	//		ignoreTeam		- True to ignore team, false otherwise.
	//		includeNeutral	- True to include neutral entity, false otherwise.
	static EntityId GetSpawnLocation(EntityId playerId, bool ignoreTeam, bool includeNeutral, EntityId groupId = -1, float minDistanceToDeath = 0.0f, float zOffset = 0.0f, Vec3 deathPos = Vec3(0,0,0));
	// <title GetSpawnLocations>
	// Syntax: GameRules.GetSpawnLocations()
	// Description:
	//		Gets the spawn locations.
	static mono::array GetSpawnLocations();
	// <title GetFirstSpawnLocation>
	// Syntax: GameRules.GetFirstSpawnLocation( int teamId )
	// Arguments:
	//		teamId - Team identifier.
	// Description:
	//		Gets the first spawn location for the team.
	static EntityId GetFirstSpawnLocation(int teamId);

	// <title AddSpawnGroup>
	// Syntax: GameRules.AddSpawnGroup(groupId)
	// Arguments:
	//		groupId - Group identifier.
	// Description:
	//		Adds a spawn group.
	static void AddSpawnGroup(EntityId groupId);
	// <title AddSpawnLocationToSpawnGroup>
	// Syntax: GameRules.AddSpawnLocationToSpawnGroup(groupId,location)
	// Arguments:
	//		groupId		- Group identifier.
	//		location	- Location.
	// Description:
	//		Add a spawn location to spawn a group.
	static void AddSpawnLocationToSpawnGroup(EntityId groupId, EntityId location);
	// <title RemoveSpawnLocationFromSpawnGroup>
	// Syntax: GameRules.RemoveSpawnLocationFromSpawnGroup(groupId,location)
	// Arguments:
	//		groupId  - Group identifier.
	//		location - Location.
	// Description:
	//		Removes a spawn location from spawn group.
	static void RemoveSpawnLocationFromSpawnGroup(EntityId groupId, EntityId location);
	// <title RemoveSpawnGroup>
	// Syntax: GameRules.RemoveSpawnGroup(groupId)
	// Arguments:
	//		groupId - Group identifier.
	// Description:
	//		Removes spawn group.
	static void RemoveSpawnGroup(EntityId groupId);
	// <title GetSpawnLocationGroup>
	// Syntax: GameRules.GetSpawnLocationGroup(spawnId)
	// Arguments:
	//		spawnId - Spawn identifier.
	// Description:
	//		Gets spawn location group.
	static EntityId GetSpawnLocationGroup(EntityId spawnId);
	// <title GetSpawnGroups>
	// Syntax: GameRules.GetSpawnGroups()
	// Description:
	//		Gets spawn groups.
	static mono::array GetSpawnGroups(int teamId = -1);
	// <title IsSpawnGroup>
	// Syntax: GameRules.IsSpawnGroup(entityId)
	// Arguments:
	//		entityId - Entity identifier.
	// Description:
	//		Checks if the entity is a spawn group.
	static bool IsSpawnGroup(EntityId entityId);

	// <title GetTeamDefaultSpawnGroup>
	// Syntax: GameRules.GetTeamDefaultSpawnGroup( int teamId )
	// Arguments:
	//		teamId - Team identifier.
	// Description:
	//		Gets team default spawn group.
	static EntityId GetTeamDefaultSpawnGroup(int teamId);
	// <title SetTeamDefaultSpawnGroup>
	// Syntax: GameRules.SetTeamDefaultSpawnGroup( int teamId, groupId)
	// Arguments:
	//		teamId  - Team identifier.
	//		groupId - Group identifier.
	// Description:
	//		Sets the default spawn group for the team.
	static void SetTeamDefaultSpawnGroup(int teamId, EntityId groupId);
	// <title SetPlayerSpawnGroup>
	// Syntax: GameRules.SetPlayerSpawnGroup(EntityId playerId, EntityId groupId)
	// Arguments:
	//		playerId - Player identifier.
	//		groupId  - Group identifier.
	// Description:
	//		Sets the player spawn group.
	static void SetPlayerSpawnGroup(EntityId playerId, EntityId groupId);

	// <title AddSpectatorLocation>
	// Syntax: GameRules.AddSpectatorLocation( EntityId location )
	// Arguments:
	//		location - Location.
	// Description:
	//		Adds a spectator location.
	static void AddSpectatorLocation(EntityId location);
	// <title RemoveSpectatorLocation>
	// Syntax: GameRules.RemoveSpectatorLocation( EntityId id )
	// Arguments:
	//		id - Spectator identifier.
	// Description:
	//		Removes a spectator location.
	static void RemoveSpectatorLocation(EntityId id);
	// <title GetSpectatorLocationCount>
	// Syntax: GameRules.GetSpectatorLocationCount(  )
	// Description:
	//		Gets the number of the spectator locations.
	static int GetSpectatorLocationCount();
	// <title GetSpectatorLocation>
	// Syntax: GameRules.GetSpectatorLocation( int idx )
	// Arguments:
	//		idx - Spectator location identifier.
	// Description:
	//		Gets the spectator location from its identifier.
	static EntityId GetSpectatorLocation(int idx);
	// <title GetSpectatorLocations>
	// Syntax: GameRules.GetSpectatorLocations(  )
	// Description:
	//		Gets the spectator locations.
	static mono::array GetSpectatorLocations();
	// <title GetRandomSpectatorLocation>
	// Syntax: GameRules.GetRandomSpectatorLocation(  )
	// Description:
	//		Gets a random spectator location.
	static EntityId GetRandomSpectatorLocation();
	// <title GetInterestingSpectatorLocation>
	// Syntax: GameRules.GetInterestingSpectatorLocation(  )
	// Description:
	//		Gets an interesting spectator location.
	static EntityId GetInterestingSpectatorLocation();
	// <title GetNextSpectatorTarget>
	// Syntax: GameRules.GetNextSpectatorTarget( EntityId playerId, int change )
	// Description:
	//		For 3rd person follow-cam mode. Gets the next spectator target.
	static EntityId GetNextSpectatorTarget(EntityId playerId, int change);
	// <title ChangeSpectatorMode>
	// Syntax: GameRules.ChangeSpectatorMode( EntityId playerId, int mode, EntityId targetId )
	// Arguments:
	//		playerId - Player identifier.
	//		mode	 - New spectator mode.
	//		targetId - Target identifier.
	// Description:
	//		Changes the spectator mode.
	static void ChangeSpectatorMode(EntityId playerId, int mode, EntityId targetId);
	// <title CanChangeSpectatorMode>
	// Syntax: GameRules.CanChangeSpectatorMode( EntityId playerId )
	// Arguments:
	//		playerId - Player identifier.
	// Description:
	//		Check if it's possible to change the spectator mode.
	static bool CanChangeSpectatorMode(EntityId playerId);
	
	// <title AddMinimapEntity>
	// Syntax: GameRules.AddMinimapEntity( EntityId entityId, int type, float lifetime )
	// Arguments:
	//		entityId - Entity identifier.
	//		type	 - .
	//		lifetime - .
	// Description:
	//		Adds a mipmap entity.
	static void AddMinimapEntity(EntityId entityId, int type, float lifetime);
	// <title RemoveMinimapEntity>
	// Syntax: GameRules.RemoveMinimapEntity( EntityId entityId )
	// Arguments:
	//		entityId - Mipmap entity player.
	// Description:
	//		Removes a mipmap entity.
	static void RemoveMinimapEntity(EntityId entityId);
	// <title ResetMinimap>
	// Syntax: GameRules.ResetMinimap(  )
	static void ResetMinimap();

	// <title GetPing>
	// Syntax: GameRules.GetPing( int channelId )
	// Arguments:
	//		channelId - Channel identifier.
	// Description:
	//		Gets the ping to a channel.
	static float GetPing(int channelId);

	// <title ResetEntities>
	// Syntax: GameRules.ResetEntities(  )
	static void ResetEntities();
	// <title ServerExplosion>
	// Syntax: GameRules.ServerExplosion( EntityId shooterId, EntityId weaponId, float dmg,
	//		Vec3 pos, Vec3 dir, float radius, float angle, float pressure, float holesize )
	static void ServerExplosion(EntityId shooterId, EntityId weaponId, float dmg, Vec3 pos, Vec3 dir, float radius, float angle, float pressure, float holesize, mono::string effect = 0, float effectScale = 1.0f, int type = 0, float minRadius = -1.0f, float minPhysRadius = -1.0f, float physRadius = -1.0f);
	// <title ServerHit>
	// Syntax: GameRules.ServerHit( EntityId targetId, EntityId shooterId, EntityId weaponId, float dmg, float radius, int materialId, int partId, int typeId )
	static void ServerHit(EntityId targetId, EntityId shooterId, EntityId weaponId, float dmg, float radius, int materialId, int partId, int typeId, Vec3 pos = Vec3(0,0,0), Vec3 dir = Vec3(0,0,0), Vec3 normal = Vec3(0,0,0));

	// <title CreateTeam>
	// Syntax: GameRules.CreateTeam( mono::string name )
	// Arguments:
	//		name - Team name.
	// Description:
	//		Creates a team.
	static int CreateTeam(mono::string name);
	// <title RemoveTeam>
	// Syntax: GameRules.RemoveTeam( int teamId )
	// Arguments:
	//		teamId - Team identifier.
	// Description:
	//		Removes the specified team.
	static void RemoveTeam(int teamId);
	// <title GetTeamName>
	// Syntax: GameRules.GetTeamName( int teamId )
	// Arguments:
	//		teamId - Team identifier.
	// Description:
	//		Gets the name of the specified team.
	static mono::string GetTeamName(int teamId);
	// <title GetTeamId>
	// Syntax: GameRules.GetTeamId( mono::string teamName )
	// Arguments:
	//		teamName - Team name.
	// Description:
	//		Gets the team identifier from the team name.
	static int GetTeamId(mono::string teamName);
	// <title GetTeamCount>
	// Syntax: GameRules.GetTeamCount(  )
	// Description:
	//		Gets the team number.
	static int GetTeamCount();
	// <title GetTeamPlayerCount>
	// Syntax: GameRules.GetTeamPlayerCount( int teamId )
	// Arguments:
	//		teamId - Team identifier.
	// Description:
	//		Gets the number of players in the specified team.
	static int GetTeamPlayerCount(int teamId, bool inGame = false);
	// <title GetTeamChannelCount>
	// Syntax: GameRules.GetTeamChannelCount( int teamId )
	// Arguments:
	//		teamId - Team identifier.
	// Description:
	//		Gets the team channel count.
	static int GetTeamChannelCount(int teamId, bool inGame = false);
	// <title GetTeamPlayers>
	// Syntax: GameRules.GetTeamPlayers( int teamId )
	// Arguments:
	//		teamId - Team identifier.
	// Description:
	//		Gets the players in the specified team.
	static mono::array GetTeamPlayers(int teamId);

	// <title SetTeam>
	// Syntax: GameRules.SetTeam( int teamId, EntityId playerId )
	// Arguments:
	//		teamId		- Team identifier.
	//		playerId	- Player identifier.
	// Description:
	//		Adds a player to a team.
	static void SetTeam(int teamId, EntityId playerId);
	// <title GetTeam>
	// Syntax: GameRules.GetTeam( EntityId playerId )
	// Arguments:
	//		playerId - Player identifier.
	// Description:
	//		Gets the team of the specified player.
	static int GetTeam(EntityId playerId);
	// <title GetChannelTeam>
	// Syntax: GameRules.GetChannelTeam( int channelId )
	// Arguments:
	//		channelId - Channel identifier.
	// Description:
	//		Gets the team in the specified channel.
	static int GetChannelTeam(int channelId);

	// <title AddObjective>
	// Syntax: GameRules.AddObjective( int teamId, mono::string objective, int status, EntityId entityId )
	// Arguments:
	//		teamId		- Team identifier.
	//		objective	- Objective name.
	//		status		- Status.
	//		entityId	- Entity identifier.
	// Description:
	//		Adds an objective for the specified team with the specified status.
	static void AddObjective(int teamId, mono::string objective, int status, EntityId entityId);
	// <title SetObjectiveStatus>
	// Syntax: GameRules.SetObjectiveStatus( int teamId, mono::string objective, int status )
	// Arguments:
	//		teamId		- Team identifier.
	//		objective	- Objective name.
	//		status		- Status.
	// Description:
	//		Sets the status of an objective.
	static void SetObjectiveStatus(int teamId, mono::string objective, int status);
	// <title SetObjectiveEntity>
	// Syntax: GameRules.SetObjectiveEntity( int teamId, mono::string objective, EntityId entityId )
	// Arguments:
	//		teamId		- Team identifier.
	//		objective	- Objective name.
	//		entityId	- Entity identifier.
	// Description:
	//		Sets the objective entity.
	static void SetObjectiveEntity(int teamId, mono::string objective, EntityId entityId);
	// <title RemoveObjective>
	// Syntax: GameRules.RemoveObjective( int teamId, mono::string objective )
	// Arguments:
	//		teamId		- Team identifier.
	//		objective	- Objective name.
	// Description:
	//		Removes an objective.
	static void RemoveObjective(int teamId, mono::string objective);
	// <title ResetObjectives>
	// Syntax: GameRules.ResetObjectives(  )
	// Description:
	//		Resets all the objectives.
	static void ResetObjectives();

	// <title ForbiddenAreaWarning>
	// Syntax: GameRules.ForbiddenAreaWarning( bool active, int timer, EntityId targetId )
	// Arguments:
	//		active		- .
	//		timer		- .
	//		targetId	- .
	// Description:
	//		Warnings for a forbidden area.
	static void ForbiddenAreaWarning(bool active, int timer, EntityId targetId);

	// <title ResetGameTime>
	// Syntax: GameRules.ResetGameTime()
	// Description:
	//		Resets the game time.
	static void ResetGameTime();
	// <title GetRemainingGameTime>
	// Syntax: GameRules.GetRemainingGameTime()
	// Description:
	//		Gets the remaining game time.
	static float GetRemainingGameTime();
	// <title IsTimeLimited>
	// Syntax: GameRules.IsTimeLimited()
	// Description:
	//		Checks if the game time is limited.
	static bool IsTimeLimited();

	// <title ResetRoundTime>
	// Syntax: GameRules.ResetRoundTime()
	// Description:
	//		Resets the round time.
	static void ResetRoundTime();
	// <title GetRemainingRoundTime>
	// Syntax: GameRules.GetRemainingRoundTime()
	// Description:
	//		Gets the remaining round time.
	static float GetRemainingRoundTime();
	// <title IsRoundTimeLimited>
	// Syntax: GameRules.IsRoundTimeLimited()
	// Description:
	//		Checks if the round time is limited.
	static bool IsRoundTimeLimited();

	// <title ResetPreRoundTime>
	// Syntax: GameRules.ResetPreRoundTime()
	// Description:
	//		Resets the pre-round time.
	static void ResetPreRoundTime();
	// <title GetRemainingPreRoundTime>
	// Syntax: GameRules.GetRemainingPreRoundTime()
	// Description:
	//		Gets the remaining pre-round time.
	static float GetRemainingPreRoundTime();

	// <title ResetReviveCycleTime>
	// Syntax: GameRules.ResetReviveCycleTime()
	// Description:
	//		Resets the revive cycle time.
	static void ResetReviveCycleTime();
	// <title GetRemainingReviveCycleTime>
	// Syntax: GameRules.GetRemainingReviveCycleTime()
	// Description:
	//		Gets the remaining cycle time.
	static float GetRemainingReviveCycleTime();

	// <title ResetGameStartTimer>
	// Syntax: GameRules.ResetGameStartTimer()
	// Description:
	//		Resets game start timer.
	static void ResetGameStartTimer(float time);
	// <title GetRemainingStartTimer>
	// Syntax: GameRules.GetRemainingStartTimer(  )
	// Description:
	//		Gets the remaining start timer.
	static float GetRemainingStartTimer();

	// <title EndGame>
	// Syntax: GameRules.EndGame()
	// Description:
	//		Ends the game.
	static void	EndGame();
	// <title NextLevel>
	// Syntax: GameRules.NextLevel()
	// Description:
	//		Loads the next level.
	static void NextLevel();

	// <title GetHitMaterialId>
	// Syntax: GameRules.GetHitMaterialId( mono::string materialName )
	// Arguments:
	//		materialName - Name of the material.
	// Description:
	//		Gets the hit material identifier from its name.
	static int GetHitMaterialId(mono::string materialName);
	// <title GetHitMaterialName>
	// Syntax: GameRules.GetHitMaterialName( int materialId )
	// Arguments:
	//		materialId - Material identifier.
	// Description:
	//		Gets the hit material name from its identifier.
	static mono::string GetHitMaterialName(int materialId);
	// <title ResetHitMaterials>
	// Syntax: GameRules.ResetHitMaterials()
	// Description:
	//		Resets the hit materials.
	static void ResetHitMaterials();

	// <title RegisterHitType>
	// Syntax: GameRules.RegisterHitType( mono::string type )
	// Arguments:
	//		type - Hit type.
	// Description:
	//		Registers a type for the hits.
	static int RegisterHitType(mono::string type);
	// <title GetHitTypeId>
	// Syntax: GameRules.GetHitTypeId( mono::string type )
	// Arguments:
	//		type - Hit type name.
	// Description:
	//		Gets a hit type identifier from the name.
	static int GetHitTypeId(mono::string type);
	// <title GetHitType>
	// Syntax: GameRules.GetHitType( int id )
	//		id	- Identifier.
	// Arguments:
	//		Gets the hit type from identifier.
	static mono::string GetHitType(int id);
	// <title ResetHitTypes>
	// Syntax: GameRules.ResetHitTypes()
	// Description:
	//		Resets the hit types.
	static void ResetHitTypes();

	// <title ForceScoreboard>
	// Syntax: GameRules.ForceScoreboard( bool force )
	// Arguments:
	//		force - True to force scoreboard, false otherwise.
	// Description:
	//		Forces the display of the scoreboard on the HUD.
	static void ForceScoreboard(bool force);
	// <title FreezeInput>
	// Syntax: GameRules.FreezeInput( bool freeze )
	// Arguments:
	//		freeze - True to freeze input, false otherwise.
	// Description:
	//		Freezes the input.
	static void FreezeInput(bool freeze);

	// <title ScheduleEntityRespawn>
	// Syntax: GameRules.ScheduleEntityRespawn( EntityId entityId, bool unique, float timer )
	// Arguments:
	//		entityId - Entity identifier.
	//		unique	 - True to have a unique respawn, false otherwise.
	//		timer	 - Float value for the respawning time.
	// Description:
	//		Schedules a respawning of the specified entity.
	static void ScheduleEntityRespawn(EntityId entityId, bool unique, float timer);
	// <title AbortEntityRespawn>
	// Syntax: GameRules.AbortEntityRespawn( EntityId entityId, bool destroyData )
	// Arguments:
	//		entityId - Entity identifier.
	//		destroyData - True to destroy the data, false otherwise.
	// Description:
	//		Aborts a respawning for the specified entity.
	static void AbortEntityRespawn(EntityId entityId, bool destroyData);

	// <title ScheduleEntityRemoval>
	// Syntax: GameRules.ScheduleEntityRemoval( EntityId entityId, float timer, bool visibility )
	// Arguments:
	//		entityId	- Entity identifier.
	//		timer		- Float value for the time of the entity removal.
	//		visibility	- Removal visibility. 
	// Description:
	//		Schedules the removal of the specified entity.
	static void ScheduleEntityRemoval(EntityId entityId, float timer, bool visibility);
	// <title AbortEntityRemoval>
	// Syntax: GameRules.AbortEntityRemoval( EntityId entityId )
	// Arguments:
	//		entityId - Entity identifier.
	// Description:
	//		Aborts the entity removal.
	static void AbortEntityRemoval(EntityId entityId);

	// <title IsDemoMode>
	// Syntax: GameRules.IsDemoMode()
	// Description:
	//		Checks if the game is running in demo mode.
	static bool IsDemoMode();


	// functions which query the console variables relevant to Crysis gamemodes.

	// <title GetTimeLimit>
	// Syntax: GameRules.GetTimeLimit()
	// Description:
	//		Gets the time limit.
	static float GetTimeLimit();
	// <title GetRoundTime>
	// Syntax: GameRules.GetRoundTime()
	// Description:
	//		Gets the round time.
	static float GetRoundTime();
	// <title GetPreRoundTime>
	// Syntax: GameRules.GetPreRoundTime()
	// Description:
	//		Gets the pre-round time.
	static int GetPreRoundTime();
	// <title GetRoundLimit>
	// Syntax: GameRules.GetRoundLimit()
	// Description:
	//		Gets the round time limit.
	static int GetRoundLimit();
	// <title GetFragLimit>
	// Syntax: GameRules.GetFragLimit()
	// Description:
	//		Gets the frag limit.
	static int GetFragLimit();
	// <title GetFragLead>
	// Syntax: GameRules.GetFragLead()
	// Description:
	//		Gets the frag lead.
	static int GetFragLead();
	// <title GetFriendlyFireRatio>
	// Syntax: GameRules.GetFriendlyFireRatio()
	// Description:
	//		Gets the friendly fire ratio.
	static float GetFriendlyFireRatio();
	// <title GetReviveTime>
	// Syntax: GameRules.GetReviveTime()
	// Description:
	//		Gets the revive time.
	static int GetReviveTime();
	// <title GetMinPlayerLimit>
	// Syntax: GameRules.GetMinPlayerLimit()
	// Description:
	//		Gets the minimum player limit.
	static int GetMinPlayerLimit();
	// <title GetMinTeamLimit>
	// Syntax: GameRules.GetMinTeamLimit()
	// Description:
	//		Gets the minimum team limit.
	static int GetMinTeamLimit();
	// <title GetTeamLock>
	// Syntax: GameRules.GetTeamLock()
	// Description:
	//		Gets the team lock.
	static int GetTeamLock();
	
	// <title IsFrozen>
	// Syntax: GameRules.IsFrozen( ScriptHandle entityId )
	// Arguments:
	//		entityId - Entity identifier.
	// Description:
	//		Checks if the entity is frozen.
	static bool IsFrozen(EntityId entityId);
	// <title FreezeEntity>
	// Syntax: GameRules.FreezeEntity( ScriptHandle entityId, bool freeze, bool vapor, bool force )
	// Arguments:
	//		entityId - Entity identifier.
	//		freeze	 - True if the entity is froozen, false otherwise.
	//		vapor	 - True to spawn vapor after freezing, false otherwise.
	//		force	 - True to force freezing even for the entity that implements "GetFrozen Amount",
	//				   false otherwise. The default value is false.
	static void FreezeEntity(EntityId entityId, bool freeze, bool vapor, bool force = false);
	// <title ShatterEntity>
	// Syntax: GameRules.ShatterEntity( ScriptHandle entityId, Vec3 pos, Vec3 impulse )
	// Arguments:
	//		entityId - Entity identifier.
	//		pos		 - Position vector.
	//		impulse	 - Impulse vector for the shattering.
	// Description:
	//		Shatters an entity.
	static void ShatterEntity(EntityId entityId, Vec3 pos, Vec3 impulse);

#ifndef _RELEASE
	// <title DebugCollisionDamage>
	// Syntax: GameRules.DebugCollisionDamage()
	// Description:
	//		Debugs collision damage.
	static int DebugCollisionDamage();
	// <title DebugHits>
	// Syntax: GameRules.DebugHits()
	// Description:
	//		Debugs hits.
	static int DebugHits();
#endif

	// <title SendHitIndicator>
	// Syntax: GameRules.SendHitIndicator( EntityId shooterId )
	// Arguments:
	//		shooterId - Shooter identifier.
	// Description:
	//		Sends a hit indicator.
	static void SendHitIndicator(EntityId shooterId);
	// <title SendDamageIndicator>
	// Syntax: GameRules.SendDamageIndicator( EntityId targetId, EntityId shooterId, EntityId weaponId )
	// Arguments:
	//		targetId	- Target identifier.
	//		shooterId	- Shooter identifier.
	//		weaponId	- Weapon identifier.
	// Description:
	//		Send a damage indicator from the shooter to the target.
	static void SendDamageIndicator(EntityId targetId, EntityId shooterId, EntityId weaponId);

	// For the power struggle tutorial.
	// <title GameOver>
	// Syntax: GameRules.GameOver( int localWinner )
	// Arguments:
	//		localWinner - Local winner ID.
	// Description:
	//		Ends the game with a local winner.
	static void GameOver(int localWinner);
	// <title EnteredGame>
	// Syntax: GameRules.EnteredGame()
	// Description:
	//		Get the game rules and enter the game.
	static void EnteredGame();
	// <title EndGameNear>
	// Syntax: GameRules.EndGameNear( EntityId entityId )
	// Arguments:
	//		entityId - Entity identifier.
	static void EndGameNear(EntityId entityId);

	// <title SPNotifyPlayerKill>
	// Syntax: GameRules.SPNotifyPlayerKill( EntityId targetId, EntityId weaponId, bool bHeadShot )
	// Arguments:
	//		targetId	- Target identifier.
	//		weaponId	- Weapon identifier.
	//		bHeadShot	- True if the shot was to the head of the target, false otherwise.
	// Description:
	//		Notifies that the player kills somebody.
	static void SPNotifyPlayerKill(EntityId targetId, EntityId weaponId, bool bHeadShot);


	// EMP Grenade

	// <title ProcessEMPEffect>
	// Syntax: GameRules.ProcessEMPEffect( EntityId targetId, float timeScale )
	// Arguments:
	//		targetId  - Target identifier.
	//		timeScale - Time scale.
	// Description:
	//		Processes the EMP (Electro Magnetic Pulse) effect.
	static void ProcessEMPEffect(EntityId targetId, float timeScale);
	// <title PerformDeadHit>
	// Syntax: GameRules.PerformDeadHit()
	// Description:
	//		Performs a death hit.
	static bool PerformDeadHit();

	// IMonoScriptBind
	virtual const char *GetClassName() { return "GameRules"; }
	// ~IMonoScriptBind

	static void RegisterGameMode(mono::string gamemode);
	static void AddGameModeAlias(mono::string gamemode, mono::string alias);
	static void AddGameModeLevelLocation(mono::string gamemode, mono::string location);
	static void SetDefaultGameMode(mono::string gamemode);

private:
	void RegisterMethods();

	static CGameRules *GetGameRules();
	static CActor *GetActor(EntityId id);
};

#endif //__SCRIPTBIND_GAMERULES_H__
