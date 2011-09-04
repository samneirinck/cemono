#include "StdAfx.h"

#include "CemonoEntityClass.h"
#include "CemonoEntityPropertyHandler.h"
#include "CemonoEntityEventHandler.h"


CCemonoEntityClass::CCemonoEntityClass(IEntityClassRegistry::SEntityClassDesc desc, const char* category, std::vector<IEntityPropertyHandler::SPropertyInfo> properties) : m_getScriptFileCallCount(0), m_pPropertyHandler(0)
{
	m_flags = desc.flags;
	m_name = desc.sName;
	m_editorHelper = desc.sEditorHelper;
	m_editorIcon = desc.sEditorIcon;
	m_category = category;
	m_properties = properties;

	m_pPropertyHandler = new CCemonoEntityPropertyHandler(properties);
}



CCemonoEntityClass::~CCemonoEntityClass()
{
	SAFE_DELETE(m_pPropertyHandler);
}

void CCemonoEntityClass::Release()
{

}

const char* CCemonoEntityClass::GetName() const
{
	return m_name;
}
uint32 CCemonoEntityClass::GetFlags() const
{
	return m_flags;
}
void CCemonoEntityClass::SetFlags(uint32 nFlags)
{
	m_flags = nFlags;
}

const char* CCemonoEntityClass::GetScriptFile() 
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
IEntityScript* CCemonoEntityClass::GetIEntityScript() const
{
	return NULL;
}
IScriptTable* CCemonoEntityClass::GetScriptTable() const
{
	return NULL;
}
const char* CCemonoEntityClass::GetEditorHelperObjectName() const
{
	return m_editorHelper;
}
const char* CCemonoEntityClass::GetEditorIconName() const
{
	return m_editorIcon;
}
bool CCemonoEntityClass::LoadScript(bool bForceReload)
{
	return false;
}
IEntityClass::UserProxyCreateFunc CCemonoEntityClass::GetUserProxyCreateFunc() const
{
	return NULL;
}

void* CCemonoEntityClass::GetUserProxyData() const
{
	return NULL;
}
IEntityPropertyHandler* CCemonoEntityClass::GetPropertyHandler() const
{
	return m_pPropertyHandler;
}


IEntityEventHandler* CCemonoEntityClass::GetEventHandler() const
{
	return new CCemonoEntityEventHandler();
}
IEntityScriptFileHandler* CCemonoEntityClass::GetScriptFileHandler() const
{
	return NULL;
}
int CCemonoEntityClass::GetEventCount()
{
	return GetEventHandler()->GetEventCount();
}
IEntityClass::SEventInfo CCemonoEntityClass::GetEventInfo( int nIndex )
{
	return IEntityClass::SEventInfo();
}
bool CCemonoEntityClass::FindEventInfo( const char *sEvent,SEventInfo &event )
{
	return false;
}

void CCemonoEntityClass::GetMemoryUsage( ICrySizer *pSizer ) const
{
	return;
}
