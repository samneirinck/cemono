#pragma once

#include <IEntityClass.h>

class CCemonoEntityClass : public IEntityClass
{
public:
	CCemonoEntityClass(IEntityClassRegistry::SEntityClassDesc desc, const char* category, std::vector<IEntityPropertyHandler::SPropertyInfo> properties);
	virtual ~CCemonoEntityClass();

	// IEntityClass interface
	virtual void Release() override;
	virtual const char* GetName() const override;
	virtual uint32 GetFlags() const override;
	virtual void SetFlags(uint32 nFlags) override;
	virtual const char* GetScriptFile() const override;
	virtual IEntityScript* GetIEntityScript() const override;
	virtual IScriptTable* GetScriptTable() const override;
	virtual const char* GetEditorHelperObjectName() const override;
	virtual const char* GetEditorIconName() const override;
	virtual bool LoadScript(bool bForceReload) override;
	virtual IEntityClass::UserProxyCreateFunc GetUserProxyCreateFunc() const override;
	virtual void* GetUserProxyData() const override;
	virtual IEntityPropertyHandler* GetPropertyHandler() const override;
	virtual IEntityEventHandler* GetEventHandler() const override;
	virtual IEntityScriptFileHandler* GetScriptFileHandler() const override;
	virtual int GetEventCount() override;
	virtual IEntityClass::SEventInfo GetEventInfo( int nIndex ) override;
	virtual bool FindEventInfo( const char *sEvent,SEventInfo &event ) override;
	virtual void GetMemoryUsage( ICrySizer *pSizer ) const override;
	// -IEntityClass

	ILINE void SetName(const char* name) { m_name = name;}
	ILINE const char* GetName() { return m_name; }

protected:
	uint32	m_flags;
	string	m_name;
	string m_editorHelper;
	string m_editorIcon;
	string m_category;
	int m_getScriptFileCallCount;
	static const int EditorFolderCall = 6;

	IEntityPropertyHandler* m_pPropertyHandler;
	std::vector<IEntityPropertyHandler::SPropertyInfo> m_properties;

};

