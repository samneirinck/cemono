#pragma once

#include <mono/metadata/object.h>

#include "IMonoAPIBinding.h"

class CMonoCVars : public IMonoAPIBinding
{
public:
	CMonoCVars();
	~CMonoCVars();

private:
	static int _GetIVal(MonoString *CVar);
	static float _GetFVal(MonoString *CVar);
	static string _GetString(MonoString *CVar);
	static void _SetCVar(MonoString *CVar, MonoString *newVal);
	static void _SetCVarIVal(MonoString *CVar, int newVal);
	static void _SetCVarFVal(MonoString *CVar, float newVal);
};

