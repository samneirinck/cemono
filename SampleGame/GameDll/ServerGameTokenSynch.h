/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2008.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: 

-------------------------------------------------------------------------
History:
- 5:9:2008   13:19 : Created by Márcio Martins

*************************************************************************/
#ifndef __SERVERGAMETOKENSYNCH_H__
#define __SERVERGAMETOKENSYNCH_H__

#if _MSC_VER > 1000
# pragma once
#endif

#include <NetHelpers.h>
#include "ClientGameTokenSynch.h"

#include "STLGlobalAllocator.h"

#include <deque>


class CServerGameTokenSynch:
	public CNetMessageSinkHelper<CServerGameTokenSynch, INetMessageSink>,
	public IGameTokenEventListener
{
public:
	CServerGameTokenSynch(IGameTokenSystem *pGTS);
	virtual ~CServerGameTokenSynch();

	void GetMemoryUsage(class ICrySizer *pSizer) const ;

	// INetMessageSink
	virtual void DefineProtocol(IProtocolBuilder *pBuilder);
	// ~INetMessageSink

	// IGameTokenEventListener
	virtual void OnGameTokenEvent(EGameTokenEvent event, IGameToken *pGameToken);
	// ~IGameTokenEventListener

	virtual void Reset();
	virtual void ResetChannel(int channelId);

	// these should only be called from the main thread
	virtual void AddToQueue(TGameTokenName name);

	virtual void AddToQueueFor(int channelId, TGameTokenName name);
	
	virtual void FullSynch(int channelId, bool reset);

	virtual void OnClientConnect(int channelId);
	virtual void OnClientDisconnect(int channelId, bool onhold);
	virtual void OnClientEnteredGame(int channelId);

	struct SChannel
	{
		SChannel()
		: local(false), pNetChannel(0), onhold(false) {};

		SChannel(INetChannel *_pNetChannel, bool isLocal)
		: local(isLocal), pNetChannel(_pNetChannel), onhold(false) {};

		INetChannel				*pNetChannel;
		SSendableHandle   lastOrderedMessage;
		bool							local:1;
		bool							onhold:1;

		void GetMemoryUsage( ICrySizer *pSizer ) const { /*nothing*/ }
	};

	SChannel *GetChannel(int channelId);
	SChannel *GetChannel(INetChannel *pNetChannel);
	int GetChannelId(INetChannel *pNetChannel) const;

protected:
	struct SChannelQueueEnt
	{
		SChannelQueueEnt() {}
		SChannelQueueEnt(int c, const TGameTokenName& k) : channel(c), name(k) {}
		int channel;
		TGameTokenName name;

		bool operator<( const SChannelQueueEnt& rhs ) const
		{
			return rhs.channel < channel || (rhs.channel == channel && rhs.name < name);
		}

		void GetMemoryUsage( ICrySizer *pSizer ) const
		{
			pSizer->AddObject(name);
		}
	};

	typedef std::map<SChannelQueueEnt, SSendableHandle>								TChannelQueueMap;
	typedef std::map<int, SChannel, std::less<int>, stl::STLGlobalAllocator<std::pair<int, SChannel> > > TChannelMap;

	TChannelQueueMap				m_queue;
	TChannelMap							m_channels;

	IGameTokenSystem *m_pGTS;
};

#endif //__SERVERGAMETOKENSYNCH_H__
