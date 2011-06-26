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

class CMono
{
public:
	CMono(void);
	virtual ~CMono(void);

	bool Init();
	void AddBinding(IMonoAPIBinding* pBinding);


private:
	bool InitializeDomain();
	bool InitializeManager();
	bool InitializeBindings();

	MonoDomain* m_pMonoDomain;
	MonoAssembly* m_pManagerAssembly;
	MonoObject* m_pManagerObject;

	std::vector<IMonoAPIBinding*> m_apiBindings;
};

