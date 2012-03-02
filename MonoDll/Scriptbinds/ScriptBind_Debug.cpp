#include "StdAfx.h"
#include "ScriptBind_Debug.h"

#include <IGameFramework.h>

CScriptBind_Debug::CScriptBind_Debug()
{
	REGISTER_METHOD(AddPersistentSphere);
	REGISTER_METHOD(AddDirection);
	REGISTER_METHOD(AddPersistentText2D);
	REGISTER_METHOD(AddPersistentLine);
}

void CScriptBind_Debug::AddPersistentSphere(Vec3 pos, float radius, ColorF color, float timeout)
{
	// TODO: Find a pretty way to do Begin in C#.
	GetIPersistentDebug()->Begin("TestAddPersistentSphere", false);
	GetIPersistentDebug()->AddSphere(pos, radius, color, timeout);
}

void CScriptBind_Debug::AddDirection(Vec3 pos, float radius, Vec3 dir, ColorF color, float timeout)
{
	GetIPersistentDebug()->AddDirection(pos, radius, dir, color, timeout);
}

void CScriptBind_Debug::AddPersistentText2D(mono::string text, float size, ColorF color, float timeout)
{
	GetIPersistentDebug()->Begin("TestAddPersistentText2D", false);
	GetIPersistentDebug()->Add2DText(ToCryString(text), size, color, timeout);
}

void CScriptBind_Debug::AddPersistentLine(Vec3 pos, Vec3 end, ColorF clr, float timeout)
{
	GetIPersistentDebug()->AddLine(pos, end, clr, timeout);
}

IPersistantDebug *CScriptBind_Debug::GetIPersistentDebug()
{
	return gEnv->pGameFramework->GetIPersistantDebug();
}