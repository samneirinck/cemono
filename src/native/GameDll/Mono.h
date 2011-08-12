#pragma once

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/appdomain.h>
#include <mono/metadata/object.h>
#include <mono/metadata/threads.h>
#include <mono/metadata/environment.h>

#include "MonoPathUtils.h"
#include "MonoClassUtils.h"
#include "MonoAPIBinding.h"

class CFGPluginManager;
class MonoAPIBinding;

class CMono
{
public:
	CMono(void);
	virtual ~CMono(void);

	bool Init();
	void AddBinding(MonoAPIBinding* pBinding);

	MonoImage* GetBclImage() { return m_pBclImage; };
	MonoAssembly* GetBclAssembly() { return m_pBclAssembly;}

	CFGPluginManager *GetFGPluginManager() const { return m_pFGPluginManager; }

private:
	MonoAssembly *LoadAssembly(const char *fullPath) { return mono_domain_assembly_open(m_pMonoDomain, fullPath); }

	bool InitializeDomain();
	bool InitializeManager();
	bool InitializeBindings();
	bool InitializeBaseClassLibraries();

private:
	CFGPluginManager* m_pFGPluginManager;

	MonoDomain* m_pMonoDomain;
	MonoAssembly* m_pManagerAssembly;
	MonoAssembly* m_pBclAssembly;
	MonoImage* m_pBclImage;
	MonoObject* m_pManagerObject;

	std::vector<MonoAPIBinding*> m_apiBindings;
};

extern CMono* g_pMono;