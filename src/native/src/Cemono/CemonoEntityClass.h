#pragma once

#include <IEntityClass.h>

class CCemonoEntityClass : public IEntityClass
{
public:
	CCemonoEntityClass(IEntityClassRegistry::SEntityClassDesc desc, const char* category, std::vector<IEntityPropertyHandler::SPropertyInfo> properties);
	virtual ~CCemonoEntityClass();

	// IEntityClass interface
	virtual void Release();
	virtual const char* GetName() const;
	virtual uint32 GetFlags() const;
	virtual void SetFlags(uint32 nFlags);
	virtual const char* GetScriptFile() const;
	virtual IEntityScript* GetIEntityScript() const;
	virtual IScriptTable* GetScriptTable() const;
	virtual const char* GetEditorHelperObjectName() const;
	virtual const char* GetEditorIconName() const;
	virtual bool LoadScript(bool bForceReload);
	virtual IEntityClass::UserProxyCreateFunc GetUserProxyCreateFunc() const;
	virtual void* GetUserProxyData() const;
	virtual IEntityPropertyHandler* GetPropertyHandler() const;
	virtual IEntityEventHandler* GetEventHandler() const;
	virtual IEntityScriptFileHandler* GetScriptFileHandler() const;
	virtual int GetEventCount();
	virtual IEntityClass::SEventInfo GetEventInfo( int nIndex );
	virtual bool FindEventInfo( const char *sEvent,SEventInfo &event );
	virtual void GetMemoryUsage( ICrySizer *pSizer ) const;
	// -IEntityClass

	ILINE void SetName(const char* name) { m_name = name;}
	ILINE const char* GetName() { return m_name; }

protected:
	uint32	m_flags;
	const char*	m_name;
	const char* m_editorHelper;
	const char* m_editorIcon;
	const char* m_category;
	int m_getScriptFileCallCount;
	static const int EditorFolderCall = 6;

	IEntityPropertyHandler* m_pPropertyHandler;
	std::vector<IEntityPropertyHandler::SPropertyInfo> m_properties;

};

