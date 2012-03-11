#include "StdAfx.h"
#include "Scriptbind_UI.h"

#include <IMonoAssembly.h>

CScriptbind_UI::CScriptbind_UI()
{
	OnReset();
}

void CScriptbind_UI::OnReset()
{
	m_pUIClass = gEnv->pMonoScriptSystem->GetCryBraryAssembly()->GetCustomClass("UI");
}

void CScriptbind_UI::OnEvent( const SUIEvent& event )
{
	m_pUIClass->CallMethod("OnEvent");
}