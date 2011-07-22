#ifndef __MONO_H__
#define __MONO_H__

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/appdomain.h>
#include <mono/metadata/object.h>
#include <mono/metadata/threads.h>
#include <mono/metadata/environment.h>

struct IMonoAPIBinding;

class CMono
{
public:
	CMono();
	virtual ~CMono();

	bool Init();
	void AddBinding(IMonoAPIBinding* pBinding);

	MonoObject *InvokeFunc(string funcName, MonoClass *pClass, bool useInstance = false, void **args = NULL);
	MonoAssembly *LoadAssembly(const char *fullPath);
private:
	bool InitializeBindings();

	// FGPS
	bool InitFGPS();
	bool RegisterPlugin(char const* pDll);

	void FreePluginLibraries();
private:
	MonoDomain *m_pMonoDomain;
	
	// CryEngine namespace
	MonoAssembly *m_pCryEngineAssembly;

	std::vector<IMonoAPIBinding*> m_apiBindings;
};

extern CMono* g_pMono;

#endif //__MONO_H__