#pragma once
#include "BaseCemonoClassBinding.h"

class CFlowSystemBinding : public BaseCemonoClassBinding
{
public:
	CFlowSystemBinding();
	virtual ~CFlowSystemBinding();

protected:
	virtual const char* GetClassName() { return "FlowManager"; }
	virtual const char* GetNamespaceExtension() { return "FlowSystem"; }

	static void _RegisterNode(MonoString* category, MonoString* nodeName);
};