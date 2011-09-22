#pragma once

#include "BaseCemonoClassBinding.h"

class CGameSystemBinding : public BaseCemonoClassBinding
{
public:
	CGameSystemBinding();
	virtual ~CGameSystemBinding();

protected:
	virtual const char* GetClassName() override { return "GameSystem"; }

	static void _RegisterGameClass(MonoObject* game);
};