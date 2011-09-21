#pragma once
#include "BaseCemonoClassBinding.h"

class CFlowSystemBinding : public BaseCemonoClassBinding
{
public:
	CFlowSystemBinding();
	virtual ~CFlowSystemBinding();

protected:
	virtual const char* GetClassName() override { return "FlowManager"; }
	virtual const char* GetNamespaceExtension() override { return "FlowSystem"; }

	static void _RegisterNode(MonoString* category, MonoString* nodeName);
};