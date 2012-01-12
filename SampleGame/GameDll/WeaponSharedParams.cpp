/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2007.
-------------------------------------------------------------------------

Description: Stores all shared weapon parameters (shared by class)...
Allows for some memory savings...

-------------------------------------------------------------------------
History:
- 30:1:2008   10:54 : Benito G.R.

*************************************************************************/

#include "StdAfx.h"
#include "Game.h"
#include "WeaponSharedParams.h"
#include "WeaponSystem.h"

CWeaponSharedParams::~CWeaponSharedParams()
{
	ResetInternal();
}

//=====================================================================
void CWeaponSharedParams::ResetInternal()
{
	m_fireParams.clear();
	m_zoomParams.clear();
}

//======================================================================
IWeaponSharedData* CWeaponSharedParams::GetZoomSharedParams(const char* name, int zoomIdx)
{
	TSharedMap::iterator it=m_zoomParams.find(zoomIdx);
	if (it!=m_zoomParams.end())
		return it->second;

	//if (true)
	{
		IWeaponSharedData *params = g_pGame->GetWeaponSystem()->CreateZoomModeData(name);
		assert(params && "CWeaponSharedParams::GetZoomSharedParams() could not create zoom mode data");

		m_zoomParams.insert(TSharedMap::value_type(zoomIdx, params));

		return params;
	}

}

//===================================================
IWeaponSharedData* CWeaponSharedParams::CreateZoomParams(const char* name)
{
	return (g_pGame->GetWeaponSystem()->CreateZoomModeData(name));
}

//==============================================================
IWeaponSharedData* CWeaponSharedParams::GetFireSharedParams(const char* name, int fireIdx)
{
	TSharedMap::iterator it=m_fireParams.find(fireIdx);
	if (it!=m_fireParams.end())
		return it->second;

	//if (true)
	{
		IWeaponSharedData *params = g_pGame->GetWeaponSystem()->CreateFireModeData(name);
		assert(params && "CWeaponSharedParams::GetFireSharedParams() could not create zoom mode data");

		m_fireParams.insert(TSharedMap::value_type(fireIdx, params));

		return params;
	}

}

//===================================================
IWeaponSharedData* CWeaponSharedParams::CreateFireParams(const char* name)
{
	return (g_pGame->GetWeaponSystem()->CreateFireModeData(name));
}

//============================================================
void CWeaponSharedParams::GetMemoryUsage(ICrySizer *s) const
{
	//AI Descriptor
	int nSize = sizeof(aiWeaponDescriptor);
	s->AddObject(&aiWeaponDescriptor,nSize);

	s->Add(aiWeaponDescriptor.smartObjectClass);
	s->Add(aiWeaponDescriptor.firecmdHandler);

	//AI weapon offsets
	s->AddContainer(aiWeaponOffsets.stanceWeponOffset);
	s->AddContainer(aiWeaponOffsets.stanceWeponOffsetLeanLeft);
	s->AddContainer(aiWeaponOffsets.stanceWeponOffsetLeanRight);

	//Zoom shared data
	s->AddContainer(m_zoomParams);
	TSharedMap::const_iterator end = m_zoomParams.end();
	for(TSharedMap::const_iterator cit = m_zoomParams.begin(); cit!=end; cit++)
	{
		cit->second->GetMemoryUsage(s);
	}

	//Fire shared data
	s->AddContainer(m_fireParams);
	TSharedMap::const_iterator end2 = m_fireParams.end();
	for(TSharedMap::const_iterator cit = m_fireParams.begin(); cit!=end2; cit++)
	{
		cit->second->GetMemoryUsage(s);
	}

}

//========================================================
CWeaponSharedParams *CWeaponSharedParamsList::GetSharedParams(const char *className, bool create)
{
	TSharedParamsMap::iterator it=m_params.find(CONST_TEMP_STRING(className));
	if (it!=m_params.end())
		return it->second;

	if (create)
	{
		CWeaponSharedParams *params=new CWeaponSharedParams();
		m_params.insert(TSharedParamsMap::value_type(className, params));

		return params;
	}

	return 0;
}

void CWeaponSharedParamsList::GetMemoryUsage(ICrySizer *s) const
{
	s->AddContainer(m_params);
	for (TSharedParamsMap::const_iterator iter = m_params.begin(); iter != m_params.end(); ++iter)
	{
		s->Add(iter->first);
		iter->second->GetMemoryUsage(s);
	}
}