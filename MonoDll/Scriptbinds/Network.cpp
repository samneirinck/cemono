#include "StdAfx.h"
#include "Network.h"

#include "MonoEntity.h"

#include <INetwork.h>

CScriptbind_Network::CScriptbind_Network()
{
	REGISTER_METHOD(RemoteInvocation);

	REGISTER_METHOD(IsMultiplayer);
	REGISTER_METHOD(IsServer);
	REGISTER_METHOD(IsClient);
}

CScriptbind_Network::~CScriptbind_Network()
{
}

void CScriptbind_Network::RemoteInvocation(EntityId entityId, EntityId targetId, mono::string methodName, mono::object args, ERMInvocation target, int channelId)
{
	CRY_ASSERT(entityId != 0);

	IGameObject *pGameObject = gEnv->pGameFramework->GetGameObject(entityId);
	CRY_ASSERT(pGameObject);

	CEntity::RMIParams params(*args, ToCryString(methodName), targetId);

	if(target & eRMI_ToServer)
		pGameObject->InvokeRMI(CEntity::SvScriptRMI(), params, target, channelId);
	else
		pGameObject->InvokeRMI(CEntity::ClScriptRMI(), params, target, channelId);
}

bool CScriptbind_Network::IsMultiplayer()
{
	return gEnv->bMultiplayer;
}

bool CScriptbind_Network::IsServer()
{
	return gEnv->bServer;
}

bool CScriptbind_Network::IsClient()
{
	return gEnv->IsClient();
}