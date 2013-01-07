#include "StdAfx.h"
#include "Debug.h"

#include <IGameFramework.h>

CScriptbind_Debug::CScriptbind_Debug()
{
	REGISTER_METHOD(AddPersistentSphere);
	REGISTER_METHOD(AddDirection);
	REGISTER_METHOD(AddPersistentText2D);
	REGISTER_METHOD(AddPersistentLine);
}

void CScriptbind_Debug::AddPersistentSphere(Vec3 pos, float radius, ColorF color, float timeout)
{
	// TODO: Find a pretty way to do Begin in C#.
	GetIPersistentDebug()->Begin("TestAddPersistentSphere", false);
	GetIPersistentDebug()->AddSphere(pos, radius, color, timeout);
}

void CScriptbind_Debug::AddDirection(Vec3 pos, float radius, Vec3 dir, ColorF color, float timeout)
{
	GetIPersistentDebug()->Begin("TestAddDirection", false);
	GetIPersistentDebug()->AddDirection(pos, radius, dir, color, timeout);
}

void CScriptbind_Debug::AddPersistentText2D(mono::string text, float size, ColorF color, float timeout)
{
	GetIPersistentDebug()->Begin("TestAddPersistentText2D", false);
	GetIPersistentDebug()->Add2DText(ToCryString(text), size, color, timeout);
}

void CScriptbind_Debug::AddPersistentLine(Vec3 pos, Vec3 end, ColorF clr, float timeout)
{
	GetIPersistentDebug()->Begin("TestAddPersistentLine", false);
	GetIPersistentDebug()->AddLine(pos, end, clr, timeout);
}

IPersistantDebug *CScriptbind_Debug::GetIPersistentDebug()
{
	return gEnv->pGameFramework->GetIPersistantDebug();
}

extern "C"
{
	_declspec(dllexport) void __cdecl LogAlways(const char *msg)
	{
		CryLogAlways(msg);
	}

	_declspec(dllexport) void __cdecl Log(const char *msg)
	{
		CryLog(msg);
	}

	_declspec(dllexport) void __cdecl Warning(const char *msg)
	{
		MonoWarning(msg); 
	}
}