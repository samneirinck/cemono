#pragma once

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/appdomain.h>
#include <mono/metadata/object.h>
#include <mono/metadata/threads.h>
#include <mono/metadata/environment.h>

class CMono
{
public:
	CMono(void);
	virtual ~CMono(void);

	bool Init();
private:

	MonoDomain* m_pMonoDomain;
};

extern CMono* g_pMono;