#ifndef __CRYMONO_PLUGIN_H__
#define __CRYMONO_PLUGIN_H__

#include <IPluginManager.h>
#include <IPluginBase.h>

#include "MonoScriptSystem.h"

#define CRYMONO_VERSION "0.6-dev"

class CCryMonoPlugin : public PluginManager::IPluginBase
{
public:
	CCryMonoPlugin() 
		: m_refs(1)
		, m_bUnloading(false)
		, m_bInitialized(false)
		, m_bFullyInitialized(false) {}

	virtual ~CCryMonoPlugin() 
	{
		m_bUnloading = true;

		if(m_bInitialized)
			m_pPluginManager->GetBase()->Release();
	}

	// IPluginBase
	virtual void AddRef() { m_refs++; }
    virtual bool Release(bool bForce = false)
	{
		if(--m_refs <= 0 || bForce)
		{
			delete this;

			return true;
		}

		return false;
	}

	virtual bool DllCanUnloadNow() { return m_refs <= 0; }
	virtual bool IsUnloading() { return m_bUnloading; }

	virtual bool IsInitialized() { return m_bInitialized; }
	virtual bool IsFullyInitialized() { return m_bFullyInitialized; }
    virtual int GetInitializationMode() const { return (int)PluginManager::IM_BeforeFramework; }

    virtual bool Check(const char *sAPIVersion) const
	{
		if(!sAPIVersion)
			return false;

		return !strcmp(sAPIVersion, "3.4.0");
	}

    virtual bool Init(SSystemGlobalEnvironment &env, SSystemInitParams &startupParams, IPluginBase *pPluginManager)
	{
		ModuleInitISystem(env.pSystem, GetName());
		m_bInitialized = true;

		m_pPluginManager = (PluginManager::IPluginManager *)pPluginManager->GetConcreteInterface(NULL);
		if(m_pPluginManager)
			pPluginManager->AddRef();
		else
			return false;

		gEnv->pMonoScriptSystem = new CScriptSystem();

		return true;
	}

	virtual bool CheckDependencies() const { return gEnv && gEnv->pSystem && m_pPluginManager && m_pPluginManager->GetBase(); }

	virtual bool InitDependencies() { m_bFullyInitialized = true; return true; }

	virtual const char *GetVersion() const { return CRYMONO_VERSION; }
	virtual const char *GetName() const { return "CryMono"; }
	virtual const char *GetCategory() const { return "General"; }

	virtual const char *ListAuthors() const { return "Filip \"i59\" Lundgren,\nins\\,\nRuan"; }

	virtual const char *ListNodes() const { return NULL; }
	virtual const char *ListCVars() const { return NULL; }
    virtual const char *ListGameObjects() const  { return NULL; }

	virtual const char *GetStatus() const { return "Undefined"; }

	virtual const char *GetCurrentConcreteInterfaceVersion() const { return CRYMONO_VERSION; }
	virtual void *GetConcreteInterface(const char *sInterfaceVersion) { return gEnv->pMonoScriptSystem; }

	void *IPluginBase::GetExtendedInterface(const char *extendedInterface) { return NULL; }
	virtual const char *GetCurrentExtendedInterfaceVersion() const { return NULL; }
	// ~IPluginBase

protected:
	int m_refs;
	bool m_bUnloading;

	bool m_bInitialized;
	bool m_bFullyInitialized;

	PluginManager::IPluginManager *m_pPluginManager;
};

#endif // __CRYMONO_PLUGIN_H__