/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2008.
-------------------------------------------------------------------------
$Id$
$DateTime$

-------------------------------------------------------------------------
History:
- 5:9:2008   13:21 : Created by Márcio Martins

*************************************************************************/
#include "StdAfx.h"
#include "ServerGameTokenSynch.h"
#include "ClientGameTokenSynch.h"
#include "Game.h"


CServerGameTokenSynch::CServerGameTokenSynch(IGameTokenSystem *pGTS)
{
	m_pGTS=pGTS;
	pGTS->RegisterListener(this);
}

CServerGameTokenSynch::~CServerGameTokenSynch()
{
	m_pGTS->UnregisterListener(this);
}


void CServerGameTokenSynch::Reset()
{
	for (TChannelMap::const_iterator it=m_channels.begin(); it!=m_channels.end(); ++it)
		ResetChannel(it->first);
}

void CServerGameTokenSynch::ResetChannel(int channelId)
{
	TChannelQueueMap::iterator cit=m_queue.lower_bound(SChannelQueueEnt(channelId, 0));
	while (cit != m_queue.end() && cit->first.channel == channelId)
	{
		TChannelQueueMap::iterator next = cit;
		++next;
		m_queue.erase(cit);
		cit = next;
	}

	if (SChannel *pChannel = GetChannel(channelId))
	{
		if (pChannel->pNetChannel)
			pChannel->pNetChannel->AddSendable( new CClientGameTokenSynch::CResetMsg(channelId), 1, &pChannel->lastOrderedMessage, &pChannel->lastOrderedMessage );
	}
}

//------------------------------------------------------------------------
void CServerGameTokenSynch::DefineProtocol(IProtocolBuilder *pBuilder)
{
	pBuilder->AddMessageSink(this, CClientGameTokenSynch::GetProtocolDef(), CServerGameTokenSynch::GetProtocolDef());
}

//------------------------------------------------------------------------
void CServerGameTokenSynch::OnGameTokenEvent(EGameTokenEvent event, IGameToken *pGameToken)
{
	if (event==EGAMETOKEN_EVENT_CHANGE)
		AddToQueue(pGameToken->GetName());
}

//------------------------------------------------------------------------
void CServerGameTokenSynch::AddToQueue(TGameTokenName name)
{
	for (TChannelMap::iterator it=m_channels.begin(); it!=m_channels.end(); ++it)
	{
		if (!it->second.local)
			AddToQueueFor(it->first, name);
	}
}

//------------------------------------------------------------------------
void CServerGameTokenSynch::AddToQueueFor(int channelId, TGameTokenName name)
{
	SChannel * pChannel = GetChannel(channelId);
	assert(pChannel);
	if (!pChannel || !pChannel->pNetChannel || pChannel->local)
		return;

	SSendableHandle& msgHdl = m_queue[SChannelQueueEnt(channelId, name)];

	TGameTokenValue value; 

	IGameToken *pToken=m_pGTS->FindToken(name.c_str());
	assert(pToken);
	if (!pToken)
		return;

	bool ok=pToken->GetValue(value);
	assert(ok);
	if (!ok)
		return;

	CClientGameTokenSynch::CSetMsg *pMsg=0;

	switch (value.GetType())
	{
	case eFDT_Int:
		pMsg=new CClientGameTokenSynch::CSetIntMsg(channelId, name, value);
		break;
	case eFDT_Float:
		pMsg=new CClientGameTokenSynch::CSetFloatMsg(channelId, name, value);
		break;
	case eFDT_EntityId:
		pMsg=new CClientGameTokenSynch::CSetEntityIdMsg(channelId, name, value);
		break;
	case eFDT_Vec3:
		pMsg=new CClientGameTokenSynch::CSetVec3Msg(channelId, name, value);
		break;
	case eFDT_String:
		pMsg=new CClientGameTokenSynch::CSetStringMsg(channelId, name, value);
		break;
	case eFDT_Bool:
		pMsg=new CClientGameTokenSynch::CSetBoolMsg(channelId, name, value);
		break;
	}

	if (pMsg)
		pChannel->pNetChannel->SubstituteSendable(pMsg, 1, &pChannel->lastOrderedMessage, &msgHdl);
	else
	{
		assert(!"Invalid type!");
	}
}

//------------------------------------------------------------------------
void CServerGameTokenSynch::FullSynch(int channelId, bool reset)
{
	if (reset)
		ResetChannel(channelId);

	IGameTokenItPtr pIt(m_pGTS->GetGameTokenIterator());
	while(IGameToken *pGameToken=pIt->Next())
	{
		if ((pGameToken->GetFlags()&EGAME_TOKEN_MODIFIED) && ((pGameToken->GetFlags()&EGAME_TOKEN_LOCALONLY)==0))
			AddToQueueFor(channelId, pGameToken->GetName());
	}
}

//------------------------------------------------------------------------
void CServerGameTokenSynch::OnClientConnect(int channelId)
{
	INetChannel *pNetChannel=g_pGame->GetIGameFramework()->GetNetChannel(channelId);

	SChannel *pChannel=GetChannel(channelId);
	if (pChannel && pChannel->onhold)
		pChannel->pNetChannel=pNetChannel;
	else
	{
		if (pChannel)
		{
			m_channels.erase(m_channels.find(channelId));
			ResetChannel(channelId);	// clear up anything in the queues.
																// the reset message won't be sent since we've deleted the channel from the map
		}
		m_channels.insert(TChannelMap::value_type(channelId, SChannel(pNetChannel, pNetChannel->IsLocal())));
	}
}

//------------------------------------------------------------------------
void CServerGameTokenSynch::OnClientDisconnect(int channelId, bool onhold)
{
	SChannel *pChannel=GetChannel(channelId);
	if (pChannel)
		pChannel->pNetChannel=0;

	if (!onhold)
		m_channels.erase(channelId);
	else if (pChannel)
		pChannel->onhold=onhold;

	ResetChannel(channelId);
}

//------------------------------------------------------------------------
void CServerGameTokenSynch::OnClientEnteredGame(int channelId)
{
	SChannel *pChannel=GetChannel(channelId);
 	if (pChannel && pChannel->pNetChannel && !pChannel->pNetChannel->IsLocal())
		FullSynch(channelId, true);
}

//------------------------------------------------------------------------
CServerGameTokenSynch::SChannel *CServerGameTokenSynch::GetChannel(int channelId)
{
	TChannelMap::iterator it=m_channels.find(channelId);
	if (it!=m_channels.end())
		return &it->second;
	return 0;
}

//------------------------------------------------------------------------
CServerGameTokenSynch::SChannel *CServerGameTokenSynch::GetChannel(INetChannel *pNetChannel)
{
	for (TChannelMap::iterator it=m_channels.begin(); it!=m_channels.end(); ++it)
		if (it->second.pNetChannel==pNetChannel)
			return &it->second;
	return 0;
}

//------------------------------------------------------------------------
int CServerGameTokenSynch::GetChannelId(INetChannel *pNetChannel) const
{
	for (TChannelMap::const_iterator it=m_channels.begin(); it!=m_channels.end(); ++it)
		if (it->second.pNetChannel==pNetChannel)
			return it->first;
	return 0;
}

void CServerGameTokenSynch::GetMemoryUsage(ICrySizer * s) const
{
	SIZER_SUBCOMPONENT_NAME(s,"ServerGameTokenSynch");
	//s->AddObject(m_queue);
	//s->AddObject(m_channels);
}
