/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2008.
-------------------------------------------------------------------------
$Id$
$DateTime$

-------------------------------------------------------------------------
History:
- 5:9:2008   13:11 : Created by Márcio Martins

*************************************************************************/
#include "StdAfx.h"
#include "ClientGameTokenSynch.h"
#include "ServerGameTokenSynch.h"


//------------------------------------------------------------------------
void CClientGameTokenSynch::DefineProtocol(IProtocolBuilder *pBuilder)
{
	pBuilder->AddMessageSink(this, CServerGameTokenSynch::GetProtocolDef(), CClientGameTokenSynch::GetProtocolDef());
}

//------------------------------------------------------------------------
NET_IMPLEMENT_IMMEDIATE_MESSAGE(CClientGameTokenSynch, ResetMsg, eNRT_ReliableOrdered, eMPF_BlocksStateChange)
{
	Reset();

	return true;
}

//------------------------------------------------------------------------
CClientGameTokenSynch::CResetMsg::CResetMsg(int _channelId)
: INetMessage(CClientGameTokenSynch::ResetMsg),
	channelId(_channelId)
{
};

//------------------------------------------------------------------------
EMessageSendResult CClientGameTokenSynch::CResetMsg::WritePayload(TSerialize ser, uint32 currentSeq, uint32 basisSeq)
{
	return eMSR_SentOk;
}

//------------------------------------------------------------------------
void CClientGameTokenSynch::CResetMsg::UpdateState(uint32 fromSeq, ENetSendableStateUpdate)
{
}

//------------------------------------------------------------------------
size_t CClientGameTokenSynch::CResetMsg::GetSize()
{
	return sizeof(*this);
};


//------------------------------------------------------------------------
// GLOBAL
//------------------------------------------------------------------------
#define DEFINE_MESSAGE(class, type, msgdef) \
	CClientGameTokenSynch::class::class(int _channelId, TGameTokenName _name, TGameTokenValue &_value) \
	:	CSetMsg(CClientGameTokenSynch::msgdef, _channelId, _name, _value) {}; \
	EMessageSendResult CClientGameTokenSynch::class::WritePayload(TSerialize ser, uint32 currentSeq, uint32 basisSeq) \
{ CClientGameTokenSynch::SerializeGameToken(ser, name, value, NTypelist::IndexOf<type, TGameTokenValueTypes>::value); \
	return eMSR_SentOk; }

#define IMPLEMENT_IMMEDIATE_MESSAGE(type) \
	TGameTokenName	name; \
	TGameTokenValue value; \
	CClientGameTokenSynch::SerializeGameToken(ser, name, value, NTypelist::IndexOf<type, TGameTokenValueTypes>::value); \
	m_pGTS->SetOrCreateToken(name, value); \
	return true;

//------------------------------------------------------------------------
NET_IMPLEMENT_IMMEDIATE_MESSAGE(CClientGameTokenSynch, SetIntMsg, eNRT_ReliableUnordered, 0)
{
	IMPLEMENT_IMMEDIATE_MESSAGE(int);
}

//------------------------------------------------------------------------
NET_IMPLEMENT_IMMEDIATE_MESSAGE(CClientGameTokenSynch, SetFloatMsg, eNRT_ReliableUnordered, 0)
{
	IMPLEMENT_IMMEDIATE_MESSAGE(float);
}


//------------------------------------------------------------------------
NET_IMPLEMENT_IMMEDIATE_MESSAGE(CClientGameTokenSynch, SetEntityIdMsg, eNRT_ReliableUnordered, 0)
{
	IMPLEMENT_IMMEDIATE_MESSAGE(EntityId);
}

//------------------------------------------------------------------------
NET_IMPLEMENT_IMMEDIATE_MESSAGE(CClientGameTokenSynch, SetVec3Msg, eNRT_ReliableUnordered, 0)
{
	IMPLEMENT_IMMEDIATE_MESSAGE(Vec3);
}

//------------------------------------------------------------------------
NET_IMPLEMENT_IMMEDIATE_MESSAGE(CClientGameTokenSynch, SetStringMsg, eNRT_ReliableUnordered, 0)
{
	IMPLEMENT_IMMEDIATE_MESSAGE(string);
}

//------------------------------------------------------------------------
NET_IMPLEMENT_IMMEDIATE_MESSAGE(CClientGameTokenSynch, SetBoolMsg, eNRT_ReliableUnordered, 0)
{
	IMPLEMENT_IMMEDIATE_MESSAGE(bool);
}

//------------------------------------------------------------------------
CClientGameTokenSynch::CSetMsg::CSetMsg(const SNetMessageDef *pDef, int _channelId, TGameTokenName &_name, TGameTokenValue &_value)
:	channelId(_channelId),
	name(_name),
	value(_value),
	INetMessage(pDef)
{
	SetGroup( 'stor' );
};

//------------------------------------------------------------------------
EMessageSendResult CClientGameTokenSynch::CSetMsg::WritePayload(TSerialize ser, uint32 currentSeq, uint32 basisSeq)
{
	return eMSR_SentOk;
}

//------------------------------------------------------------------------
void CClientGameTokenSynch::CSetMsg::UpdateState(uint32 fromSeq, ENetSendableStateUpdate)
{
}

//------------------------------------------------------------------------
size_t CClientGameTokenSynch::CSetMsg::GetSize()
{
	return sizeof(*this);
};


DEFINE_MESSAGE(CSetIntMsg,			int,			SetIntMsg);
DEFINE_MESSAGE(CSetFloatMsg,		float,		SetFloatMsg);
DEFINE_MESSAGE(CSetVec3Msg,			Vec3,			SetVec3Msg);
DEFINE_MESSAGE(CSetEntityIdMsg, EntityId, SetEntityIdMsg);
DEFINE_MESSAGE(CSetStringMsg,		string,		SetStringMsg);
DEFINE_MESSAGE(CSetBoolMsg,			bool,			SetBoolMsg);

#undef DEFINE_MESSAGE
#undef IMPLEMENT_IMMEDIATE_MESSAGE


//------------------------------------------------------------------------
void CClientGameTokenSynch::SerializeGameToken(TSerialize ser, TGameTokenName &name, TGameTokenValue &value, int type)
{
	ser.Value("name", name);

	switch (type)
	{
	case eFDT_Int:
		{
			int i;
			if (ser.IsWriting())
				i=*value.GetPtr<int>();
			ser.Value("value", i);
			if (ser.IsReading())
				value.Set(i);
		}
		break;
	case eFDT_Float:
		{
			float f;
			if (ser.IsWriting())
				f=*value.GetPtr<float>();
			ser.Value("value", f);
			if (ser.IsReading())
				value.Set(f);
		}
		break;
	case eFDT_Vec3:
		{
			Vec3 v;
			if (ser.IsWriting())
				v=*value.GetPtr<Vec3>();
			ser.Value("value", v);
			if (ser.IsReading())
				value.Set(v);
		}
		break;
	case eFDT_EntityId:
		{
			EntityId e;
			if (ser.IsWriting())
				e=*value.GetPtr<EntityId>();
			ser.Value("value", e);
			if (ser.IsReading())
				value.Set(e);
		}
		break;
	case eFDT_String:
		{
			static string s;
			s.resize(0);
			if (ser.IsWriting())
				s=*value.GetPtr<string>();
			ser.Value("value", s);
			if (ser.IsReading())
				value.Set(s);
		}
		break;
	case eFDT_Bool:
		{
			bool b;
			if (ser.IsWriting())
				b=*value.GetPtr<bool>();
			ser.Value("value", b);
			if (ser.IsReading())
				value.Set(b);
		}
		break;
	default:
		assert(0);
		break;
	}
}


//------------------------------------------------------------------------
void CClientGameTokenSynch::GetMemoryUsage(ICrySizer *pSizer) const
{
	pSizer->Add(*this);
}
