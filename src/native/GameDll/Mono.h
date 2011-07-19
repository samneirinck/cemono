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
#include "IMonoAPIBinding.h"

struct IMonoAPIBinding;

class CMono
{
public:
	CMono(void);
	virtual ~CMono(void);

	bool Init();
	void AddBinding(IMonoAPIBinding* pBinding);

	MonoImage* GetBclImage() { return m_pBclImage; };


private:
	bool InitializeDomain();
	bool InitializeManager();
	bool InitializeBindings();
	bool InitializeBaseClassLibraries();

	MonoDomain* m_pMonoDomain;
	MonoAssembly* m_pManagerAssembly;
	MonoAssembly* m_pBclAssembly;
	MonoImage* m_pBclImage;
	MonoObject* m_pManagerObject;

	std::vector<IMonoAPIBinding*> m_apiBindings;
};

extern CMono* g_pMono;