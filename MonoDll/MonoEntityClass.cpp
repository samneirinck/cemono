#include "StdAfx.h"

#include "MonoEntityClass.h"
#include "MonoEntityPropertyHandler.h"
#include "MonoEntityEventHandler.h"


CMonoEntityClass::CMonoEntityClass(IEntityClassRegistry::SEntityClassDesc desc, const char* category, std::vector<IEntityPropertyHandler::SPropertyInfo> properties) : m_pPropertyHandler(0)
{
	m_flags = desc.flags;
	m_name = desc.sName;
	m_editorHelper = desc.sEditorHelper;
	m_editorIcon = desc.sEditorIcon;
	m_category = category;
	m_properties = properties;

	m_pPropertyHandler = new CMonoEntityPropertyHandler(properties);
	m_pEventHandler = new CMonoEntityEventHandler();
}

CMonoEntityClass::~CMonoEntityClass()
{
	SAFE_DELETE(m_pPropertyHandler);
	SAFE_DELETE(m_pEventHandler);
}

const char* CMonoEntityClass::GetScriptFile() const
{
	// This is a *horrible* workaround to be able to have entities in folders in the editor
	// Only on the 6th call of this method we would like to return a dummy path, which then makes the correct directory structure in the editor
	// On all other methods, we return an empty string. If we don't, then our IEntityPropertyHandler is not used, instead it will try to use the properties of the scriptfile specified by GetScriptFile()
	//++m_getScriptFileCallCount;
	//if (m_getScriptFileCallCount == EditorFolderCall)
	//{
	//	return m_category;
	//} else {
		return "";
	//}
}

IEntityScript* CMonoEntityClass::GetIEntityScript() const
{
	return NULL;
}

IScriptTable* CMonoEntityClass::GetScriptTable() const
{
	return NULL;
}

bool CMonoEntityClass::LoadScript(bool bForceReload)
{
	return false;
}

IEntityClass::UserProxyCreateFunc CMonoEntityClass::GetUserProxyCreateFunc() const
{
	return NULL;
}

void* CMonoEntityClass::GetUserProxyData() const
{
	return NULL;
}

IEntityPropertyHandler* CMonoEntityClass::GetPropertyHandler() const
{
	return m_pPropertyHandler;
}

IEntityEventHandler* CMonoEntityClass::GetEventHandler() const
{
	return m_pEventHandler;
}

IEntityScriptFileHandler* CMonoEntityClass::GetScriptFileHandler() const
{
	return NULL;
}

int CMonoEntityClass::GetEventCount()
{
	return m_pEventHandler->GetEventCount();
}

IEntityClass::SEventInfo CMonoEntityClass::GetEventInfo( int nIndex )
{
	return IEntityClass::SEventInfo();
}

bool CMonoEntityClass::FindEventInfo( const char *sEvent,SEventInfo &event )
{
	return false;
}

void CMonoEntityClass::GetMemoryUsage( ICrySizer *pSizer ) const
{
	return;
}