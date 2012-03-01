#include "StdAfx.h"
#include "ScriptBind_Debug.h"

#include <IGameFramework.h>

CScriptBind_Debug::CScriptBind_Debug()
{
	REGISTER_METHOD(AddPersistantSphere);
	REGISTER_METHOD(AddDirection);
	REGISTER_METHOD(AddPersistantText2D);
}

void CScriptBind_Debug::AddPersistantSphere(Vec3 pos, float radius, ColorF color, float timeout)
{
	// TODO: Find a pretty way to do Begin in C#.
	GetIPersistantDebug()->Begin("TestAddPersistantSphere", false);
	GetIPersistantDebug()->AddSphere(pos, radius, color, timeout);
}

void CScriptBind_Debug::AddDirection(Vec3 pos, float radius, Vec3 dir, ColorF color, float timeout)
{
	GetIPersistantDebug()->AddDirection(pos, radius, dir, color, timeout);
}

void CScriptBind_Debug::AddPersistantText2D(mono::string text, float size, ColorF color, float timeout)
{
	GetIPersistantDebug()->Begin("TestAddPersistantText2D", false);
	GetIPersistantDebug()->Add2DText(ToCryString(text), size, color, timeout);
}

IPersistantDebug *CScriptBind_Debug::GetIPersistantDebug()
{
	return gEnv->pGameFramework->GetIPersistantDebug();
}