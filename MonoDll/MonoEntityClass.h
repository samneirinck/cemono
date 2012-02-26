/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// Entity class dummy.
//////////////////////////////////////////////////////////////////////////
// 08/12/2011 : Created by Filip 'i59' Lundgren (Based on version by ins\)
////////////////////////////////////////////////////////////////////////*/
#ifndef __MONO_ENTITY_CLASS_H__
#define __MONO_ENTITY_CLASS_H__

#include <IEntityClass.h>

class CEntityClass : public IEntityClass
{
public:
	CEntityClass(IEntityClassRegistry::SEntityClassDesc desc, const char* category, std::vector<IEntityPropertyHandler::SPropertyInfo> properties);
	virtual ~CEntityClass();

	// IEntityClass interface
	virtual void Release() { delete this; }
	virtual const char *GetName() const { return m_name; }
	virtual uint32 GetFlags() const { return m_flags; }
	virtual void SetFlags(uint32 nFlags) { m_flags = nFlags; }
	virtual const char *GetScriptFile() const { return ""; }
	virtual IEntityScript *GetIEntityScript() const { return NULL; }
	virtual IScriptTable *GetScriptTable() const { return NULL; }
	virtual const char *GetEditorHelperObjectName() const { return m_editorHelper; }
	virtual const char *GetEditorIconName() const { return m_editorIcon; }
	virtual bool LoadScript(bool bForceReload) { return false; }
	virtual IEntityClass::UserProxyCreateFunc GetUserProxyCreateFunc() const { return NULL; }
	virtual void *GetUserProxyData() const { return NULL; }
	virtual IEntityPropertyHandler *GetPropertyHandler() const { return m_pPropertyHandler;  }
	virtual IEntityEventHandler *GetEventHandler() const { return m_pEventHandler; }
	virtual IEntityScriptFileHandler *GetScriptFileHandler() const { return NULL; }
	virtual int GetEventCount();
	virtual IEntityClass::SEventInfo GetEventInfo( int nIndex ) { return IEntityClass::SEventInfo(); }
	virtual bool FindEventInfo( const char *sEvent,SEventInfo &event ) { return false; }
	virtual void GetMemoryUsage( ICrySizer *pSizer ) const {}
	// ~IEntityClass

	ILINE void SetName(const char* name) { m_name = name;}
	ILINE const char* GetName() { return m_name; }

protected:
	uint32	m_flags;
	string	m_name;
	string m_editorHelper;
	string m_editorIcon;
	string m_category;

	IEntityPropertyHandler *m_pPropertyHandler;
	IEntityEventHandler *m_pEventHandler;

	std::vector<IEntityPropertyHandler::SPropertyInfo> m_properties;
};

#endif //__MONO_ENTITY_CLASS_H__