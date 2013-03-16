#ifndef __CRYSCRIPTINSTANCE_H__
#define __CRYSCRIPTINSTANCE_H__

#include "MonoScriptSystem.h"
#include "MonoObject.h"

class CCryScriptInstance 
	: public CScriptObject
	, public IMonoScriptEventListener
{
public:
	CCryScriptInstance(EMonoScriptFlags flags);
	~CCryScriptInstance();

	// IMonoScriptEventListener
	virtual void OnReloadStart();
	virtual void OnReloadComplete();

	virtual void OnScriptInstanceCreated(const char *scriptName, EMonoScriptFlags scriptType, IMonoObject *pScriptInstance) {}
	virtual void OnScriptInstanceInitialized(IMonoObject *pScriptInstance) {}
	virtual void OnScriptInstanceReleased(IMonoObject *pScriptInstance, int scriptId) {}

	virtual void OnShutdown() { Release(); }
	// ~IMonoScriptEventListener

	// CScriptObject
	virtual void SetManagedObject(MonoObject *newObject, bool allowGC) override;

	virtual void Release(bool triggerGC = true) override;
	// ~CScriptObject

	int GetScriptId() { return m_scriptId; }

private:
	int m_scriptId;
	EMonoScriptFlags m_flags;
};

#endif //__CRYSCRIPTINSTANCE_H__