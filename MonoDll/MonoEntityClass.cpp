#include "StdAfx.h"

#include "MonoEntityClass.h"
#include "MonoEntityPropertyHandler.h"
#include "MonoEntityEventHandler.h"

CEntityClass::CEntityClass(IEntityClassRegistry::SEntityClassDesc desc, SMonoEntityPropertyInfo *pProperties, int numProperties)
{
	m_flags = desc.flags;
	m_name = desc.sName;
	m_classInfo = desc.editorClassInfo;

	m_pPropertyHandler = new CEntityPropertyHandler(pProperties, numProperties);
	m_pEventHandler = new CEntityEventHandler();

	m_proxyCreateFunc = desc.pUserProxyCreateFunc;
}

CEntityClass::~CEntityClass()
{
	SAFE_DELETE(m_pPropertyHandler);
	SAFE_DELETE(m_pEventHandler);
}

int CEntityClass::GetEventCount()
{
	return m_pEventHandler->GetEventCount();
}