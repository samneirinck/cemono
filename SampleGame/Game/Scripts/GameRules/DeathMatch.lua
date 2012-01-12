--------------------------------------------------------------------------
--	Crytek Source File.
-- 	Copyright (C), Crytek Studios, 2001-2009.
--------------------------------------------------------------------------
--	$Id$
--	$DateTime$
--	Description: GameRules implementation for Death Match
--               Refactored from the Instant Action game rule
--  
--------------------------------------------------------------------------
--  History:
--  - 22/ 9/2004   16:20 : Created by Mathieu Pinard
--  - 04/10/2004   10:43 : Modified by Craig Tiller
--  - 07/10/2004   16:02 : Modified by Marcio Martins
--  - 05/11/2009   12:00 : Modified by Mathieu Pinard and Pau Novau
--
----------------------------------------------------------------------------------------------------
Script.LoadScript( "scripts/gamerules/campaign.lua", 1, 1 );
--------------------------------------------------------------------------
DeathMatch = new( Campaign );
DeathMatch.States = { "Reset", "PreGame", "InGame", "PostGame", };

----------------------------------------------------------------------------------------------------
DeathMatch.START_TIMER = 0; -- starting countdown timer
DeathMatch.INVULNERABILITY_TIME = 5;	-- player invulnerability time when spawning. 0 to disable
DeathMatch.WEAPON_ABANDONED_TIME = 45;

DeathMatch.NEXTLEVEL_TIMERID = 1050;
DeathMatch.NEXTLEVEL_TIME = 12000;
DeathMatch.ENDGAME_TIMERID = 1040;
DeathMatch.ENDGAME_TIME = 3000;
DeathMatch.TICK_TIMERID = 1010;
DeathMatch.TICK_TIME = 1000;

DeathMatch.SCORE_KILLS_KEY = 100;
DeathMatch.SCORE_DEATHS_KEY = 101;
DeathMatch.SCORE_HEADSHOTS_KEY = 102;

DeathMatch.ENDGAMETYPE_TIMELIMIT = 2;
DeathMatch.ENDGAMETYPE_SCORELIMIT = 3;

DeathMatch.PLAYER_START_EQUIPEMENTPACK_NAME = "Multiplayer";

----------------------------------------------------------------------------------------------------
DeathMatch.DamagePlayerToPlayer =
{
	helmet    = 1.25,
	kevlar    = 1.15,

	head      = 2.25,
	torso     = 1.15,
	arm_left  = 0.85,
	arm_right = 0.85,
	leg_left  = 0.85,
	leg_right = 0.85,

	foot_left  = 0.70,
	foot_right = 0.70,
	hand_left  = 0.70,
	hand_right = 0.70,
	assist_min = 0.8,
};

----------------------------------------------------------------------------------------------------
Net.Expose {
	Class = DeathMatch,
	ClientMethods = {
		ClVictory              = { RELIABLE_ORDERED, POST_ATTACH, ENTITYID, },
		ClNoWinner             = { RELIABLE_ORDERED, POST_ATTACH, },

		ClClientConnect        = { RELIABLE_UNORDERED, POST_ATTACH, STRING, BOOL },
		ClClientDisconnect     = { RELIABLE_UNORDERED, POST_ATTACH, STRING, },
		ClClientEnteredGame    = { RELIABLE_UNORDERED, POST_ATTACH, STRING, },
	},
	ServerMethods = {
		RequestRevive          = { RELIABLE_UNORDERED, POST_ATTACH, ENTITYID, },
		RequestSpectatorTarget = { RELIABLE_UNORDERED, POST_ATTACH, ENTITYID, INT8 },
	},
	ServerProperties = {
	},
};

----------------------------------------------------------------------------------------------------
function SimpleLog( s )
	--Log( s );
end

----------------------------------------------------------------------------------------------------
function DeathMatch:IsMultiplayer()
	return true;
end

----------------------------------------------------------------------------------------------------
function DeathMatch:CheckPlayerScoreLimit( playerId, score )
	if ( self:GetState() and self:GetState() ~= "InGame" ) then
		return;
	end

	local scoreLimit = self.game:GetFragLimit();
	
	local isScoreLimitedGame = ( 0 < scoreLimit );
	if ( not isScoreLimitedGame ) then
		return;
	end
	
	if ( score < scoreLimit ) then
		return;
	end
	
	local scoreLead = self.game:GetFragLead();
	
	local isScoreMarginNeeded = ( 0 < scoreLead );
	if ( isScoreMarginNeeded ) then
		local players = self.game:GetPlayers( true );
		if ( players ) then
			for i, player in pairs( players ) do
				local isCheckedPlayer = ( player.id == playerId );
				if ( not isCheckedPlayer ) then
					local playerMeetsMargin = ( self:GetPlayerScore( player.id ) + scoreLead <= score );
					if ( not playerMeetsMargin ) then
						return;
					end
				end
			end
		end
	end
	
	self:OnGameEnd( playerId, ENDGAMETYPE_SCORELIMIT );
end

----------------------------------------------------------------------------------------------------
function DeathMatch:FindHighestScorePlayerId()

	local players = self.game:GetPlayers( true );
	if ( not players ) then
		return nil;
	end
	
	local draw = false;
	local maxScoreId = nil;
	local maxScore = nil;
	for i, player in pairs( players ) do
		local score = self:GetPlayerScore( player.id );
		if ( not maxScore or maxScore < score ) then
			draw = false;
			maxScore = score;
			maxScoreId = player.id;
		elseif ( maxScore == score ) then
			draw = true;
		end
	end
	
	if ( not draw ) then
		return maxScoreId;
	end
	
	-- if there's a draw, check for lowest number of deaths	
	local minDeaths = nil;
	local minDeathsId = nil;
	for i, player in pairs( players ) do
		local score = self:GetPlayerScore( player.id );
		if ( score == maxScore ) then
			local deaths = self:GetPlayerDeaths( player.id );
			if ( not minDeaths or deaths < minDeaths ) then
				draw = false;
				minDeaths = deaths;
				minDeathsId = player.id;
			elseif ( minDeaths == deaths ) then
				draw = true;
			end
		end
	end
	
	if ( not draw ) then
		return minDeathsId;
	end

	return nil;
end

----------------------------------------------------------------------------------------------------
function DeathMatch:CheckTimeLimit()
	if ( self:GetState() and self:GetState() ~= "InGame" ) then
		return;
	end
	
	if ( not self.game:IsTimeLimited() ) then
		return;
	end
	
	if ( 0 <= self.game:GetRemainingGameTime() ) then
		return;
	end
	
	local winnerId = FindHighestScorePlayerId();

	self:OnGameEnd( winnerId, ENDGAMETYPE_TIMELIMIT );
end

----------------------------------------------------------------------------------------------------
function DeathMatch:PlayerCountOk()
	local preGame = ( self:GetState() == "PreGame" );
	
	local playerCount = self.game:GetPlayerCount( preGame );
	return ( 0 < playerCount );
end

----------------------------------------------------------------------------------------------------
function DeathMatch:OnGameEnd( winningPlayerId, endGameType )
	SimpleLog( "[ DeathMatch ] OnGameEnd" );
	if ( winningPlayerId ) then
		local playerName = EntityName( winningPlayerId );
		self.game:SendTextMessage( TextMessageCenter, "@mp_GameOverWinner", TextMessageToAll, nil, playerName );
		self.allClients:ClVictory( winningPlayerId );
	else
		self.game:SendTextMessage( TextMessageCenter, "@mp_GameOverNoWinner", TextMessageToAll );
		self.allClients:ClNoWinner();
	end
	
	self.game:EndGame();

	self:GotoState( "PostGame" );
end

----------------------------------------------------------------------------------------------------
function DeathMatch:ResetTime()
	SimpleLog( "[ DeathMatch ] ResetTime" );
	self.game:ResetGameTime();
end

----------------------------------------------------------------------------------------------------
function DeathMatch.Server:OnInit()
	SimpleLog( "[ DeathMatch ] Server:OnInit" );
	Campaign.Server.OnInit( self );
	
	self.isServer = CryAction.IsServer();
	self.isClient = CryAction.IsClient();
	
	self:Reset( true );
end

----------------------------------------------------------------------------------------------------
function DeathMatch.Server:OnStartGame()
	SimpleLog( "[ DeathMatch ] Server:OnStartGame" );
	self:StartTicking();
end

----------------------------------------------------------------------------------------------------
function DeathMatch:Reset( forcePregame )
	SimpleLog( "[ DeathMatch ] Reset" );
	self:ResetTime();

	--if ( self.forceInGame ) then
	--	self:GotoState( "InGame" );
	--end
	
	if ((self:PlayerCountOk() and (not forcePregame)) or (self.forceInGame)) then
		self:GotoState("InGame");
	else
		self:GotoState("PreGame");
	end
	self.forceInGame=nil;
end

----------------------------------------------------------------------------------------------------
function DeathMatch:OnReset()
	SimpleLog( "[ DeathMatch ] OnReset" );
	if ( self.isServer and self.Server.OnReset ) then
		self.Server.OnReset( self );
	end

	if ( self.isClient and self.Client.OnReset ) then
		self.Client.OnReset( self );
	end
end

----------------------------------------------------------------------------------------------------
function DeathMatch.Server:OnReset()
	SimpleLog( "[ DeathMatch ] Server:OnReset" );
	self:Reset( true );
end

----------------------------------------------------------------------------------------------------
function DeathMatch.Client:OnReset()
	SimpleLog( "[ DeathMatch ] Client:OnReset" );
end

----------------------------------------------------------------------------------------------------
function DeathMatch:RestartGame( forceInGame )
	SimpleLog( "[ DeathMatch ] RestartGame" );	
	self:GotoState( "Reset" );

	self.game:ResetEntities();

	if ( forceInGame ) then
		self.forceInGame = true;
	end
end

----------------------------------------------------------------------------------------------------
function DeathMatch.Client:OnActorAction( player, action, activation, value )

	if ( ( action == "attack1" ) and ( activation == "press" ) ) then
		if ( player:IsDead() ) then
			self.server:RequestRevive( player.id );
			return false;
		end
	end

	return true;
end

----------------------------------------------------------------------------------------------------
function DeathMatch.Client:OnDisconnect( cause, desc )
	SimpleLog( "[ DeathMatch ] Client:OnDisconnect" );	
--	Game.ShowMainMenu();
end

----------------------------------------------------------------------------------------------------
function DeathMatch:SpawnPlayer( channelId, name )
	SimpleLog( "[ DeathMatch ] SpawnPlayer" );
	local pos = g_Vectors.temp_v1;
	local angles = g_Vectors.temp_v2;
	ZeroVector( pos );
	ZeroVector( angles );
	
	-- TODO: Create player in a nicer position to start with?
		
	local player = self.game:SpawnPlayer( channelId, name or "Nomad", "Player", pos, angles );	

	return player;
end

----------------------------------------------------------------------------------------------------
function DeathMatch.Server:OnClientConnect(channelId, reset, name)
	local player = self:SpawnPlayer( channelId, name );
	
	local channelOnHold = CryAction.IsChannelOnHold( channelId )
	if ( not channelOnHold ) then
		self:ResetScore( player.id );
	end
	
	--if ( reset ) then
		--Log( "[ DeathMatch ] Reseting player %d", channelId );
		self:RevivePlayer( player.actor:GetChannel(), player );
	--else
	--	Log( "[ DeathMatch ] Not reseting player %d", channelId );
	--	local reconnect = channelOnHold;
	--	self.game:ChangeSpectatorMode( player.id, 2, NULL_ENTITY );
	--	self.otherClients:ClClientConnect( channelId, player:GetName(), reconnect );
	--end
	
	return player;
end

----------------------------------------------------------------------------------------------------
function DeathMatch.Server:OnClientDisconnect(channelId)
	local player=self.game:GetPlayerByChannelId(channelId);
	
	self.otherClients:ClClientDisconnect(channelId, player:GetName());
end

----------------------------------------------------------------------------------------------------
function DeathMatch.Server:OnClientEnteredGame( channelId, player, reset )
	--Log( "[ DeathMatch ] Server:OnClientEnteredGame()" );
	
	local channelOnHold = CryAction.IsChannelOnHold( channelId );
	
	--if ( not reset ) then
		local playerAlive = ( 0 < player.actor:GetHealth() );
		if ( playerAlive ) then
			player.actor:SetPhysicalizationProfile( "alive" );
		else
			player.actor:SetPhysicalizationProfile( "ragdoll" );
		end
		
		
	--end
	
	--if ((not onHold) and (not reset)) then
	--	self.game:ChangeSpectatorMode(player.id, 2, NULL_ENTITY);
	--elseif (not reset) then
	--	if (player.actor:GetHealth()>0) then
	--		player.actor:SetPhysicalizationProfile("alive");
	--	else
	--		player.actor:SetPhysicalizationProfile("ragdoll");
	--	end
	--end

	--if (not reset) then
		self.otherClients:ClClientEnteredGame( channelId, player:GetName() );
	--end
end

----------------------------------------------------------------------------------------------------
function DeathMatch.Server:OnUpdate( frameTime )
end

----------------------------------------------------------------------------------------------------
function DeathMatch:StartTicking(client)
	SimpleLog( "[ DeathMatch ] StartTicking" );
	if ((not client) or (not self.isServer)) then
		self:SetTimer(self.TICK_TIMERID, self.TICK_TIME);
	end
end

----------------------------------------------------------------------------------------------------
function DeathMatch.Server:OnTimer(timerId, msec)
	SimpleLog( "[ DeathMatch ] Server:OnTimer" );
	if (timerId==self.TICK_TIMERID) then
		if (self.OnTick) then
			--pcall(self.OnTick, self);
			self:OnTick();
			self:SetTimer(self.TICK_TIMERID, self.TICK_TIME);
		end
	elseif(timerId==self.NEXTLEVEL_TIMERID) then
		self:GotoState("Reset");
		self.game:NextLevel();
	end
end

----------------------------------------------------------------------------------------------------
function DeathMatch.Client:OnTimer(timerId, msec)
	SimpleLog( "[ DeathMatch ] Client:OnTimer" );
	if (timerId == self.TICK_TIMERID) then
		self:OnClientTick();
		if (not self.isServer) then
			self:SetTimer(self.TICK_TIMERID, self.TICK_TIME);
		end
	elseif (timerId == self.ENDGAME_TIMERID) then
		self:EndGame(true);
	end
end

----------------------------------------------------------------------------------------------------
function DeathMatch.Client:OnUpdate( frameTime )
	SimpleLog( "[ DeathMatch ] Client:OnUpdate" );
	Campaign.Client.OnUpdate( self, frameTime );
	
	self:UpdateScores();
end

----------------------------------------------------------------------------------------------------
function DeathMatch:ShowScores( enable )
	SimpleLog( "[ DeathMatch ] ShowScores" );
	self.showScores = enable;
end

----------------------------------------------------------------------------------------------------
function DeathMatch:EndGame( enable )
	SimpleLog( "[ DeathMatch ] EndGame" );
	self.showScores = enable;
	self.game:ForceScoreboard( enable );
	self.game:FreezeInput( enable );
end

----------------------------------------------------------------------------------------------------
function DeathMatch:UpdateScores()
	SimpleLog( "[ DeathMatch ] UpdateScores" );
	if ( not self.showScores ) then
		return;
	end
	
	if ( not g_localActor ) then
		return;
	end
	
	local players = self.game:GetPlayers();
	if ( not players ) then
		return;
	end
	
	g_localActor.actor:ResetScores();
	for i, player in ipairs( players ) do
		local kills = self.game:GetSynchedEntityValue( player.id, self.SCORE_KILLS_KEY );
		local deaths = self.game:GetSynchedEntityValue( player.id, self.SCORE_DEATHS_KEY );
		
		g_localActor.actor:RenderScore( player.id, kills or 0, deaths or 0, 0 );
	end	
end

----------------------------------------------------------------------------------------------------
function DeathMatch:CanRevive( playerId )
	SimpleLog( "[ DeathMatch ] CanRevive" );
	local player = System.GetEntity( playerId );
	if ( not player ) then
		return false;
	end
	
	return true;
end

----------------------------------------------------------------------------------------------------
function DeathMatch:ResetPlayerEquip( actor )
	if ( self.game:IsDemoMode() ~= 0 ) then 
		return;
	end

	actor.inventory:Destroy();

	ItemSystem.GiveItemPack( actor.id, self.PLAYER_START_EQUIPEMENTPACK_NAME, true, true );
end

----------------------------------------------------------------------------------------------------
function DeathMatch:RevivePlayer( channelId, player, keepEquip )
	SimpleLog( "[ DeathMatch ] RevivePlayer" );
	local spawnSuccess = false;
	
	if ( player:IsDead() ) then
		keepEquip = false;
	end
	
	local spawnId;
	local zOffset;
	spawnId, zOffset = self.game:GetSpawnLocation( player.id, true, true, NULL_ENTITY, 50, player.death_pos );
	
	if ( spawnId ) then
		local spawn = System.GetEntity( spawnId )
		if ( spawn ) then
			spawn:Spawned( player );

			local pos = spawn:GetWorldPos( g_Vectors.temp_v1 );
			pos.z = pos.z + zOffset;
			
			local angles = spawn:GetWorldAngles( g_Vectors.temp_v2 );
		
			local teamId = self.game:GetTeam( player.id );
			self.game:RevivePlayer( player.id, pos, angles, teamId, not keepEquip );

			spawnSuccess = true;
		end
	end
	
	-- make the game realise the areas we're in right now...
	-- otherwise we'd have to wait for an entity system update, next frame
	player:UpdateAreas();

	if ( spawnSuccess ) then
		if ( player.actor:GetSpectatorMode() ~= 0 ) then
			player.actor:SetSpectatorMode( 0, NULL_ENTITY );
		end
	
		if ( not keepEquip ) then
			self:ResetPlayerEquip( player );
		end
		player.deathTime = nil;		
		
		if ( self.INVULNERABILITY_TIME and 0 < self.INVULNERABILITY_TIME ) then
			self.game:SetInvulnerability( player.id, true, self.INVULNERABILITY_TIME );
		end
	end
	
	return spawnSuccess;
end

----------------------------------------------------------------------------------------------------
function DeathMatch:ProcessActorDamage( hit )
	SimpleLog( "[ DeathMatch ] ProcessActorDamage" );
	local target = hit.target;
	local health = target.actor:GetHealth();
	
	health = math.floor( health - hit.damage );
	
	target.actor:SetHealth( health );

	local isActorDead = ( health <= 0 );
	return isActorDead;
end

----------------------------------------------------------------------------------------------------
function DeathMatch:CalcExplosionDamage( entity, explosion, obstruction )
	SimpleLog( "[ DeathMatch ] CalcExplosionDamage" );
	local newDamage = Campaign.CalcExplosionDamage( self, entity, explosion, obstruction );

	return newDamage;
end

----------------------------------------------------------------------------------------------------
function DeathMatch.Server:OnPlayerKilled(hit)
	SimpleLog( "[ DeathMatch ] Server:OnPlayerKilled" );
	local target=hit.target;
	target.deathTime=_time;
	target.deathPos=target:GetWorldPos(target.deathPos);
	
	self.game:KillPlayer(hit.targetId, true, true, hit.shooterId, hit.weaponId, hit.damage, hit.materialId, hit.typeId, hit.dir);
	self:ProcessScores(hit);
end

----------------------------------------------------------------------------------------------------
function DeathMatch.Client:OnKill( playerId, shooterId, weaponClassName, damage, material, hit_type )
	SimpleLog( "[ DeathMatch ] Client:OnKill" );
	--TODO: Show FX depending on hit type?
end

----------------------------------------------------------------------------------------------------
function DeathMatch:ProcessDeath(hit)
	SimpleLog( "[ DeathMatch ] ProcessDeath" );
	self.Server.OnPlayerKilled(self, hit);
end

----------------------------------------------------------------------------------------------------
function DeathMatch:ResetScore( playerId )
	SimpleLog( "[ DeathMatch ] ResetScore" );
	self.game:SetSynchedEntityValue( playerId, self.SCORE_KILLS_KEY, 0 );
	self.game:SetSynchedEntityValue( playerId, self.SCORE_DEATHS_KEY, 0 );
	self.game:SetSynchedEntityValue( playerId, self.SCORE_HEADSHOTS_KEY, 0 );
	
	CryAction.SendGameplayEvent( playerId, eGE_ScoreReset, "", 0 );
end

----------------------------------------------------------------------------------------------------
function DeathMatch:GetPlayerScore( playerId )
	return self.game:GetSynchedEntityValue( playerId, self.SCORE_KILLS_KEY, 0 ) or 0;
end

----------------------------------------------------------------------------------------------------
function DeathMatch:GetPlayerDeaths( playerId )
	return self.game:GetSynchedEntityValue( playerId, self.SCORE_DEATHS_KEY, 0 ) or 0;
end

----------------------------------------------------------------------------------------------------
function DeathMatch:Award(player, deaths, kills, headshots)
	SimpleLog( "[ DeathMatch ] Award" );
	if (player) then
		local ckills=kills + (self.game:GetSynchedEntityValue(player.id, self.SCORE_KILLS_KEY) or 0);
		local cdeaths=deaths + (self.game:GetSynchedEntityValue(player.id, self.SCORE_DEATHS_KEY) or 0);
		local cheadshots=headshots + (self.game:GetSynchedEntityValue(player.id, self.SCORE_HEADSHOTS_KEY) or 0);
		
		self.game:SetSynchedEntityValue(player.id, self.SCORE_KILLS_KEY, ckills);
		self.game:SetSynchedEntityValue(player.id, self.SCORE_DEATHS_KEY, cdeaths);
		self.game:SetSynchedEntityValue(player.id, self.SCORE_HEADSHOTS_KEY, cheadshots);
		
		if (kills and kills~=0) then
			CryAction.SendGameplayEvent(player.id, eGE_Scored, "kills", ckills);
		end
				
		if (deaths and deaths~=0) then
			CryAction.SendGameplayEvent(player.id, eGE_Scored, "deaths", cdeaths);
		end
		
		if (headshots and headshots~=0) then
			CryAction.SendGameplayEvent(player.id, eGE_Scored, "headshots", cheadshots);
		end

		if (self.CheckPlayerScoreLimit) then
			self:CheckPlayerScoreLimit(player.id, ckills);
		end
	end
end

----------------------------------------------------------------------------------------------------
function DeathMatch:ProcessScores(hit)
	SimpleLog( "[ DeathMatch ] ProcessScores" );
	local target=hit.target;
	local shooter=hit.shooter;
	local headshot=self:IsHeadShot(hit);

	local h=0;
	if (headshot) then
		h=1;
	end
	
	if (target.actor and target.actor:IsPlayer()) then
		self:Award(target, 1, 0, 0);
	end
	
	if (shooter and shooter.actor and shooter.actor:IsPlayer()) then
		if (target ~= shooter) then
			self:Award(shooter, 0, 1, h);
		else
			self:Award(shooter, 0, -1, 0);
		end
	end
end

----------------------------------------------------------------------------------------------------
function DeathMatch.Server:RequestRevive(entityId)
	SimpleLog( "[ DeathMatch ] RequestRevive" );
	local player = System.GetEntity( entityId );

	if ( player and player.actor ) then
		if (player.deathTime and _time-player.deathTime>2.5 and player:IsDead()) then
			self:RevivePlayer(player.actor:GetChannel(), player);
		end
	end
end

----------------------------------------------------------------------------------------------------
function DeathMatch.Server:RequestSpectatorTarget(playerId, change)
	SimpleLog( "[ DeathMatch ] RequestSpectatorTarget" );
	local targetId = self.game:GetNextSpectatorTarget(playerId, change);
	if(targetId) then
		if(targetId~=0) then
			local player = System.GetEntity(playerId);
			self.game:ChangeSpectatorMode(playerId, 3, targetId);
		elseif(self.game:GetTeam(playerId) == 0) then
			self.game:ChangeSpectatorMode(playerId, 1, NULL_ENTITY);	-- noone to spectate, so revert to free look mode
		end
	end
end

----------------------------------------------------------------------------------------------------
function DeathMatch.Server:OnItemPickedUp( itemId, actorId )
	self.game:AbortEntityRemoval( itemId );
end

----------------------------------------------------------------------------------------------------
function DeathMatch.Server:OnItemDropped( itemId, actorId )
	self.game:ScheduleEntityRemoval( itemId, self.WEAPON_ABANDONED_TIME, false );
end

----------------------------------------------------------------------------------------------------
function DeathMatch:GetServerStateTable()
	return self.Server[ self:GetState() ];
end

----------------------------------------------------------------------------------------------------
function DeathMatch:GetClientStateTable()
	return self.Client[ self:GetState() ];
end

----------------------------------------------------------------------------------------------------
function DeathMatch:OnTick()
	local onTickFunction = self:GetServerStateTable().OnTick;
	if ( onTickFunction ) then
		onTickFunction( self );
	end
end

----------------------------------------------------------------------------------------------------
function DeathMatch:OnClientTick()
	SimpleLog( "[ DeathMatch ] OnClientTick" );
	local onTickFunction = self:GetClientStateTable().OnTick;
	if ( onTickFunction ) then
		onTickFunction( self );
	end
end

----------------------------------------------------------------------------------------------------
function DeathMatch:ResetPlayers()
	SimpleLog( "[ DeathMatch ] ResetPlayers" );
	self:ReviveAllPlayers();
	
	local players=self.game:GetPlayers();
	if (players) then
		for i,player in pairs(players) do
			self:ResetScore(player.id);		
		end
	end
end

----------------------------------------------------------------------------------------------------
function DeathMatch:ReviveAllPlayers(keepEquip)
	SimpleLog( "[ DeathMatch ] ReviveAllPlayers" );
	local players=self.game:GetPlayers();

	if (players) then
		for i,player in ipairs(players) do
			if (player and player.actor and player.actor:GetSpectatorMode()==0) then
				self:RevivePlayer(player.actor:GetChannel(), player, keepEquip);
			end
		end
	end
end

----------------------------------------------------------------------------------------------------
function DeathMatch:DefaultState(cs, state)
	local default=self[cs];
	self[cs][state]={
		OnClientConnect = default.OnClientConnect,
		OnClientDisconnect = default.OnClientDisconnect,
		OnClientEnteredGame = default.OnClientEnteredGame,
		OnDisconnect = default.OnDisconnect, -- client only
		OnActorAction = default.OnActorAction, -- client only
		OnStartLevel = default.OnStartLevel,
		OnStartGame = default.OnStartGame,
		
		OnKill = default.OnKill,
		OnHit = default.OnHit,
		OnExplosion = default.OnExplosion,
		RequestSpectatorTarget = default.RequestSpectatorTarget,
		OnItemPickedUp = default.OnItemPickedUp,
		OnItemDropped = default.OnItemDropped,

		OnTimer = default.OnTimer,
		OnUpdate = default.OnUpdate,	
	}
end

----------------------------------------------------------------------------------------------------
DeathMatch:DefaultState( "Server", "Reset" );
DeathMatch:DefaultState( "Client", "Reset" );

----------------------------------------------------------------------------------------------------
DeathMatch:DefaultState( "Server", "PreGame" );
DeathMatch:DefaultState( "Client", "PreGame" );

----------------------------------------------------------------------------------------------------
DeathMatch:DefaultState( "Server", "InGame" );
DeathMatch:DefaultState( "Client", "InGame" );

----------------------------------------------------------------------------------------------------
DeathMatch:DefaultState( "Server", "PostGame" );
DeathMatch:DefaultState( "Client", "PostGame" );

----------------------------------------------------------------------------------------------------
function DeathMatch.Client.PreGame:OnBeginState()
	SimpleLog( "[ DeathMatch ] Client.PreGame:OnBeginState" );
	self:StartTicking(true);
end

----------------------------------------------------------------------------------------------------
function DeathMatch.Server.PreGame:OnBeginState()
	SimpleLog( "[ DeathMatch ] Server.PreGame:OnBeginState" );
	self:ResetTime();	
	self:StartTicking();
	self:ResetPlayers();
		
	self.starting = false;
end

----------------------------------------------------------------------------------------------------
function DeathMatch.Server.PreGame:OnTick()
	if ( self:PlayerCountOk() ) then
		if ( not self.starting ) then
			self.starting = true;
			self.game:ResetGameStartTimer( self.START_TIMER );
		end
	else
		if ( self.starting ) then
			self.starting = false;
			self.game:ResetGameStartTimer( -1 );
		end
	end
	
	if ( self.starting ) then
		if ( self.game:GetRemainingStartTimer() <= 0 ) then
			self.starting = false;
			self:RestartGame( true );
		end
	end
end

----------------------------------------------------------------------------------------------------
function DeathMatch.Client.PreGame:OnTick()
	local remainingTimeToStart = math.floor( self.game:GetRemainingStartTimer() + 0.5 );
	if ( 0 < remainingTimeToStart ) then
		self.game:TextMessage( TextMessageCenter,  "@mp_GameStartingCountdown", remainingTimeToStart );
	end
end

----------------------------------------------------------------------------------------------------
function DeathMatch.Server.InGame:OnTick()
end

----------------------------------------------------------------------------------------------------
function DeathMatch.Server.InGame:OnBeginState()
	SimpleLog( "[ DeathMatch ] Server.InGame:OnBeginState" );
	self:ResetTime();
	self:StartTicking();
	self:ResetPlayers();
	
	CryAction.SendGameplayEvent(NULL_ENTITY, eGE_GameStarted, "", 1);--server
end

----------------------------------------------------------------------------------------------------
function DeathMatch.Client.InGame:OnBeginState()
	SimpleLog( "[ DeathMatch ] Client.InGame:OnBeginState" );
	self:StartTicking(true);
	
	CryAction.SendGameplayEvent(NULL_ENTITY, eGE_GameStarted, "", 0);--client
end

----------------------------------------------------------------------------------------------------
function DeathMatch.Server.InGame:OnUpdate(frameTime)
	SimpleLog( "[ DeathMatch ] Server.InGame:OnUpdate" );
	DeathMatch.Server:OnUpdate( frameTime );

	self:CheckTimeLimit();
end

----------------------------------------------------------------------------------------------------
function DeathMatch.Server.PostGame:OnBeginState()
	SimpleLog( "[ DeathMatch ] Server.PostGame:OnBeginState" );
	CryAction.SendGameplayEvent(NULL_ENTITY, eGE_GameEnd, "", 1);--server
	
	self:StartTicking();
	self:SetTimer(self.NEXTLEVEL_TIMERID, self.NEXTLEVEL_TIME);
end

----------------------------------------------------------------------------------------------------
function DeathMatch.Client.PostGame:OnBeginState()
	SimpleLog( "[ DeathMatch ] Client.PostGame:OnBeginState" );
	CryAction.SendGameplayEvent(NULL_ENTITY, eGE_GameEnd, "", 0);--client
	
	self:StartTicking(true);
	self:SetTimer(self.ENDGAME_TIMERID, self.ENDGAME_TIME);
end

----------------------------------------------------------------------------------------------------
function DeathMatch.Client.PostGame:OnEndState()
	SimpleLog( "[ DeathMatch ] Client.PostGame:OnEndState" );
	self:EndGame(false);
end

----------------------------------------------------------------------------------------------------
function DeathMatch.Server:OnPlayerKilled(hit)
	SimpleLog( "[ DeathMatch ] OnPlayerKilled" );
	local target=hit.target;
	target.deathTime=_time;
	target.deathPos=target:GetWorldPos(target.deathPos);
	
	self.game:KillPlayer(hit.targetId, true, true, hit.shooterId, hit.weaponId, hit.damage, hit.materialId, hit.typeId, hit.dir);
	self:ProcessScores(hit);
end

----------------------------------------------------------------------------------------------------
function DeathMatch.Client:ClVictory( winningPlayerId )
	if ( not winningPlayerId or winningPlayerId == 0 ) then
		return;
	end
	
	if ( winningPlayerId == g_localActorId ) then
		self.game:GameOver( 1 );
	else
		self.game:GameOver( -1 );
	end
end

----------------------------------------------------------------------------------------------------
function DeathMatch.Client:ClClientConnect(name, reconnect)
end

----------------------------------------------------------------------------------------------------
function DeathMatch.Client:ClClientDisconnect(name)
end

----------------------------------------------------------------------------------------------------
function DeathMatch.Client:ClClientEnteredGame(name)
end

----------------------------------------------------------------------------------------------------
function DeathMatch.Client:ClNoWinner()
	self.game:GameOver( 0 );
end

