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

struct SMonoEntityPropertyInfo;

class CEntityClass : public IEntityClass
{
public:
	CEntityClass(IEntityClassRegistry::SEntityClassDesc desc, SMonoEntityPropertyInfo *pProperties, int numProperties);
	virtual ~CEntityClass();

	// IEntityClass interface
	virtual void Release() { delete this; }
	virtual const char *GetName() const { return m_classDesc.sName; }
	virtual uint32 GetFlags() const { return m_classDesc.flags; }
	virtual void SetFlags(uint32 nFlags) { m_classDesc.flags = nFlags; }
	virtual const char *GetScriptFile() const { return ""; }
	virtual IEntityScript *GetIEntityScript() const { return nullptr; }
	virtual IScriptTable *GetScriptTable() const { return nullptr; }
	virtual const SEditorClassInfo& GetEditorClassInfo() const { return m_classDesc.editorClassInfo; }
	virtual void SetEditorClassInfo(const SEditorClassInfo& editorClassInfo) { m_classDesc.editorClassInfo = editorClassInfo; }
	virtual const char *GetEditorHelperObjectName() const { return m_classDesc.editorClassInfo.sHelper; }
	virtual const char *GetEditorIconName() const { return m_classDesc.editorClassInfo.sIcon; }
	virtual bool LoadScript(bool bForceReload) { return false; }
	virtual UserProxyCreateFunc GetUserProxyCreateFunc() const { return m_proxyCreateFunc; }
	virtual void *GetUserProxyData() const { return nullptr; }
	virtual IEntityPropertyHandler *GetPropertyHandler() const { return m_pPropertyHandler;  }
	virtual IEntityEventHandler *GetEventHandler() const { return m_pEventHandler; }
	virtual IEntityScriptFileHandler *GetScriptFileHandler() const { return nullptr; }
	virtual int GetEventCount();
	virtual IEntityClass::SEventInfo GetEventInfo( int nIndex ) { return IEntityClass::SEventInfo(); }
	virtual bool FindEventInfo( const char *sEvent,SEventInfo &event ) { return false; }
	virtual void GetMemoryUsage( ICrySizer *pSizer ) const {}
	// ~IEntityClass

protected:
	UserProxyCreateFunc m_proxyCreateFunc;

	IEntityPropertyHandler *m_pPropertyHandler;
	IEntityEventHandler *m_pEventHandler;

	IEntityClassRegistry::SEntityClassDesc m_classDesc;
};

#endif //__MONO_ENTITY_CLASS_H__