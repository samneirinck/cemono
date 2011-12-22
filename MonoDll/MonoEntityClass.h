#ifndef __MONO_ENTITY_CLASS_H__
#define __MONO_ENTITY_CLASS_H__

#include <IEntityClass.h>

class CMonoEntityClass : public IEntityClass
{
public:
	CMonoEntityClass(IEntityClassRegistry::SEntityClassDesc desc, const char* category, std::vector<IEntityPropertyHandler::SPropertyInfo> properties);
	virtual ~CMonoEntityClass();

	// IEntityClass interface
	virtual void Release() { delete this; }
	virtual const char* GetName() const { return m_name; }
	virtual uint32 GetFlags() const { return m_flags; }
	virtual void SetFlags(uint32 nFlags) { m_flags = nFlags; }
	virtual const char* GetScriptFile() const;
	virtual IEntityScript* GetIEntityScript() const;
	virtual IScriptTable* GetScriptTable() const;
	virtual const char* GetEditorHelperObjectName() const { return m_editorHelper; }
	virtual const char* GetEditorIconName() const { return m_editorIcon; }
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
	string	m_name;
	string m_editorHelper;
	string m_editorIcon;
	string m_category;
	static const int EditorFolderCall = 6;

	IEntityPropertyHandler *m_pPropertyHandler;
	IEntityEventHandler *m_pEventHandler;

	std::vector<IEntityPropertyHandler::SPropertyInfo> m_properties;
};

#endif //__MONO_ENTITY_CLASS_H__