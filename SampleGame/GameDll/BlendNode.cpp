/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2008.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description:  BlendNodes and BlendGroups for ScreenEffects

							-------------------------------------------------------------------------
History:
- 23:1:2008   Created by Benito G.R. - Refactor'd from John N. ScreenEffects.h/.cpp

*************************************************************************/

#include "StdAfx.h"
#include "BlendNode.h"
#include "BlendTypes.h"
#include "BlendedEffect.h"

#define MIN_BLEND_GROUP_SIZE	8  

//------------------CBlendNode-----------------------------

CBlendJobNode::CBlendJobNode():
m_speed(0.0f),
m_progress(0.0f),
m_myEffect(NULL),
m_blendType(NULL)
{
}

//--------------------------------------
void CBlendJobNode::Init(IBlendType* pBlend, IBlendedEffect* pFx, float speed)
{
	assert(speed>0.001f);		// otherwise the effect would go on forever

	m_blendType = pBlend;
	m_myEffect	= pFx;
	m_speed			= speed;
}
//-------------------------------------------------
CBlendJobNode::~CBlendJobNode()
{
	ResetBlendJob();
}

//--------------------------------------------
void CBlendJobNode::ResetBlendJob()
{
	if(m_myEffect)
		m_myEffect->Release();

	if(m_blendType)
		m_blendType->Release();

	m_myEffect = NULL; m_blendType = NULL;
	m_speed = m_progress = 0.0f;
}

//-----------------------------------------------
void CBlendJobNode::Update(float frameTime)
{
	float progressDifferential = m_speed * frameTime;
	m_progress = min(m_progress + progressDifferential, 1.0f);
	
	float point = 0.1f;
	if(m_blendType)
		point = m_blendType->Blend(m_progress);
	if(m_myEffect)
		m_myEffect->Update(point);
}

//-------------------CBlendGroup----------------------------

CBlendGroup::CBlendGroup()
{
	AllocateMinJobs();
}

//---------------------------------
CBlendGroup::~CBlendGroup()
{
	for(int i=0;i<m_jobs.size();i++)
	{
		delete m_jobs[i];
	}
}

//---------------------------------
void CBlendGroup::AllocateMinJobs()
{
	m_jobs.reserve(MIN_BLEND_GROUP_SIZE);

	for(int i=0;i<MIN_BLEND_GROUP_SIZE;i++)
	{
		CBlendJobNode *node = new CBlendJobNode();
		m_jobs.push_back(node);
	}
}
//----------------------------------------
void CBlendGroup::Update(float frameTime)
{
	TJobVector::iterator it, end=m_jobs.end();

	for(it=m_jobs.begin();it!=end;++it)
	{
		CBlendJobNode *pNode = *it;

		if(!pNode->IsFree())
		{
			pNode->Update(frameTime);

			if(pNode->Done())
				pNode->ResetBlendJob();
		}
	}
}

//--------------------------------------
bool CBlendGroup::HasJobs()
{
	// inefficient - refactoring of the container handling should be done rater than this function
	TJobVector::const_iterator it, end=m_jobs.end();

	for(it=m_jobs.begin();it!=end;++it)
	{
		const CBlendJobNode *pNode = *it;

		if(!pNode->IsFree())
			return true;
	}

	return false;
}

//---------------------------------------------
void CBlendGroup::AddJob(IBlendType* pBlend, IBlendedEffect* pFx, float speed)
{
	TJobVector::iterator it, end=m_jobs.end();

	for(it=m_jobs.begin();it!=end;++it)
	{
		CBlendJobNode *pNode = *it;

		if(pNode->IsFree())
		{
			pNode->Init(pBlend,pFx,speed);
			pFx->Init();
			return;
		}
	}

	//We need to add another slot
	CBlendJobNode* newJob = new CBlendJobNode();
	newJob->Init(pBlend,pFx,speed);
	pFx->Init();

	m_jobs.push_back(newJob);
}

//-----------------------------------
void CBlendGroup::Reset()
{
	TJobVector::iterator it, end=m_jobs.end();

	for(it=m_jobs.begin();it!=end;++it)
	{
		CBlendJobNode *pNode = *it;
		
		pNode->ResetBlendJob();	
	}
}

//------------------------------------
void CBlendGroup::GetMemoryUsage(ICrySizer* s) const
{
	s->Add(this);
	s->AddContainer(m_jobs);

	for(int i=0;i<m_jobs.size();i++)
		s->Add(m_jobs[i]);
}