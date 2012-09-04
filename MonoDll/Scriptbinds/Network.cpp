#include "StdAfx.h"
#include "Network.h"

#include "MonoEntity.h"

#include <INetwork.h>

CNetwork::CNetwork()
{
	REGISTER_METHOD(RemoteInvocation);
}

CNetwork::~CNetwork()
{
}

void CNetwork::RemoteInvocation(EntityId entityId, int scriptId, mono::string methodName, mono::object args, ERMInvocation target, int channelId)
{
	CRY_ASSERT(entityId != 0);

	IGameObject *pGameObject = gEnv->pGameFramework->GetGameObject(entityId);
	CRY_ASSERT(pGameObject);

	CEntity::RMIParams params(*args, ToCryString(methodName), scriptId);

	if(target & eRMI_ToServer)
		pGameObject->InvokeRMI(CEntity::SvScriptRMI(), params, target, channelId);
	else
		pGameObject->InvokeRMI(CEntity::ClScriptRMI(), params, target, channelId);
}