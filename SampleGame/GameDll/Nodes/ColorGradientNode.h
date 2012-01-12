/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2009.
-------------------------------------------------------------------------
History:
- 12:05:2009   Created by Federico Rebora
*************************************************************************/

#pragma once

#ifndef COLOR_GRADIENT_NODE_H
#define COLOR_GRADIENT_NODE_H

#include "G2FlowBaseNode.h"

//namespace EngineFacade
//{
//	struct IGameEnvironment;
//}

class CFlowNode_ColorGradient : public CFlowBaseNode
{
public:
	static const SInputPortConfig inputPorts[];

	CFlowNode_ColorGradient( SActivationInfo* activationInformation);
	~CFlowNode_ColorGradient();

	virtual void GetConfiguration(SFlowNodeConfig& config);
	virtual void ProcessEvent(EFlowEvent event, SActivationInfo* activationInformation);
	virtual void GetMemoryUsage(ICrySizer* sizer) const;

	enum EInputPorts
	{
		eInputPorts_Trigger,
		eInputPorts_TexturePath,
		eInputPorts_TransitionTime,
		eInputPorts_Count,
	};

private:
//	IGameEnvironment& m_environment;
	ITexture *m_pTexture;
};

#endif 
