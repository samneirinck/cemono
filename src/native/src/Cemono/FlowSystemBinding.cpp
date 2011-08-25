#include "stdafx.h"
#include "FlowSystemBinding.h"

CFlowSystemBinding::CFlowSystemBinding()
{
	REGISTER_METHOD(_RegisterNode);
}

CFlowSystemBinding::~CFlowSystemBinding()
{
}

void CFlowSystemBinding::_RegisterNode(MonoString* category, MonoString* nodeName)
{
	// TODO: Do something
}