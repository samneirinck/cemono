/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2008.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: 

-------------------------------------------------------------------------
History:
- 5:9:2008   13:01 : Created by Márcio Martins

*************************************************************************/
#ifndef __CLIENTGAMETOKENSYNCH_H__
#define __CLIENTGAMETOKENSYNCH_H__

#if _MSC_VER > 1000
# pragma once
#endif

#include <NetHelpers.h>
#include <IGameTokens.h>



typedef string								TGameTokenName;
typedef TFlowInputData				TGameTokenValue;
typedef TFlowSystemDataTypes	TGameTokenValueTypes;



#define DECLARE_MESSAGE(classname) \
class classname: public CSetMsg \
	{ \
	public: \
	classname(int _channelId, TGameTokenName _name, TGameTokenValue &_value); \
	EMessageSendResult WritePayload(TSerialize ser, uint32 currentSeq, uint32 basisSeq); \
	}; \


struct IGameFramework;
class CServerGameTokenSynch;

class CClientGameTokenSynch:
	public CNetMessageSinkHelper<CClientGameTokenSynch, INetMessageSink>
{
public:
	CClientGameTokenSynch(IGameTokenSystem *pGTS) { m_pGTS=pGTS; };
	virtual ~CClientGameTokenSynch() {};

	void Reset() {}; // TODO: implement
	void GetMemoryUsage(ICrySizer *pSizer) const ;

	// INetMessageSink
	virtual void DefineProtocol(IProtocolBuilder *pBuilder);
	// ~INetMessageSink

	//------------------------------------------------------------------------
	class CResetMsg: public INetMessage
	{
	public:
		CResetMsg(int _channelId);
		
		int											channelId;

		EMessageSendResult WritePayload(TSerialize ser, uint32 currentSeq, uint32 basisSeq);
		void UpdateState(uint32 fromSeq, ENetSendableStateUpdate update);
		size_t GetSize();
	};

	//------------------------------------------------------------------------
	class CSetMsg: public INetMessage
	{
	public:
		CSetMsg(const SNetMessageDef *pDef, int _channelId, TGameTokenName &_name, TGameTokenValue &_value);

		int							channelId;

		TGameTokenName	name;
		TGameTokenValue	value;

		virtual EMessageSendResult WritePayload(TSerialize ser, uint32 currentSeq, uint32 basisSeq);
		virtual void UpdateState(uint32 fromSeq, ENetSendableStateUpdate update);
		virtual size_t GetSize();
	};

	static void SerializeGameToken(TSerialize ser, TGameTokenName &name, TGameTokenValue &value, int type);
	
	DECLARE_MESSAGE(CSetIntMsg);
	DECLARE_MESSAGE(CSetFloatMsg);
	DECLARE_MESSAGE(CSetVec3Msg);
	DECLARE_MESSAGE(CSetEntityIdMsg);
	DECLARE_MESSAGE(CSetStringMsg);
	DECLARE_MESSAGE(CSetBoolMsg);

	//------------------------------------------------------------------------
	NET_DECLARE_IMMEDIATE_MESSAGE(ResetMsg);
	
	NET_DECLARE_IMMEDIATE_MESSAGE(SetIntMsg);
	NET_DECLARE_IMMEDIATE_MESSAGE(SetFloatMsg);
	NET_DECLARE_IMMEDIATE_MESSAGE(SetVec3Msg);
	NET_DECLARE_IMMEDIATE_MESSAGE(SetEntityIdMsg);
	NET_DECLARE_IMMEDIATE_MESSAGE(SetStringMsg);
	NET_DECLARE_IMMEDIATE_MESSAGE(SetBoolMsg);

protected:
	IGameTokenSystem *m_pGTS;
};

#undef DECLARE_MESSAGE

#endif //__CLIENTGAMETOKENSYNCH_H__
