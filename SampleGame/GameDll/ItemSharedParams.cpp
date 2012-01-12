/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2007.
-------------------------------------------------------------------------
$Id$
$DateTime$

-------------------------------------------------------------------------
History:
- 3:4:2005   15:11 : Created by Márcio Martins

*************************************************************************/
#include "StdAfx.h"
#include "ItemSharedParams.h"


void CItemSharedParams::GetMemoryUsage(ICrySizer *s) const
{
	s->AddContainer(actions);
	s->AddContainer(accessoryparams);
	s->AddContainer(helpers);
	s->AddContainer(layers);
	s->AddContainer(dualWieldSupport);

	params.GetMemoryUsage(s);

	for (CItem::TActionMap::const_iterator iter = actions.begin(); iter != actions.end(); ++iter)
		s->Add(iter->first);
	for (CItem::THelperVector::const_iterator iter = helpers.begin(); iter != helpers.end(); ++iter)
		iter->GetMemoryUsage(s);
	for (CItem::TLayerMap::const_iterator iter = layers.begin(); iter != layers.end(); ++iter)
	{
		s->Add(iter->first);
		iter->second.GetMemoryUsage(s);
	}
	for (CItem::TAccessoryParamsMap::const_iterator iter = accessoryparams.begin(); iter != accessoryparams.end(); ++iter)
	{
		s->Add(iter->first);
		iter->second.GetMemoryUsage(s);
	}
	for (CItem::TDualWieldSupportMap::const_iterator iter = dualWieldSupport.begin(); iter != dualWieldSupport.end(); ++iter)
		s->Add(iter->first);
}

CItemSharedParams *CItemSharedParamsList::GetSharedParams(const char *className, bool create)
{
	TSharedParamsMap::iterator it=m_params.find(CONST_TEMP_STRING(className));
	if (it!=m_params.end())
		return it->second;

	if (create)
	{
		CItemSharedParams *params=new CItemSharedParams();
		m_params.insert(TSharedParamsMap::value_type(className, params));

		return params;
	}

	return 0;
}

void CItemSharedParamsList::GetMemoryUsage(ICrySizer *s) const
{
	s->AddContainer(m_params);
	for (TSharedParamsMap::const_iterator iter = m_params.begin(); iter != m_params.end(); ++iter)
	{
		s->Add(iter->first);
		iter->second->GetMemoryUsage(s);
	}
}