#include "StdAfx.h"

#include "MonoEntityClass.h"
#include "MonoEntityPropertyHandler.h"
#include "MonoEntityEventHandler.h"

CEntityClass::CEntityClass(IEntityClassRegistry::SEntityClassDesc desc, const char* category, std::vector<IEntityPropertyHandler::SPropertyInfo> properties)
	: m_pPropertyHandler(NULL)
{
	m_flags = desc.flags;
	m_name = desc.sName;
	m_editorHelper = desc.sEditorHelper;
	m_editorIcon = desc.sEditorIcon;
	m_category = category;
	m_properties = properties;

	m_pPropertyHandler = new CEntityPropertyHandler(properties);
	m_pEventHandler = new CEntityEventHandler();
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