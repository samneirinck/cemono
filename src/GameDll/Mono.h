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
class CFGPluginManager;

class CMono
{
public:
	CMono();
	virtual ~CMono();

	bool Init();
	void AddBinding(IMonoAPIBinding* pBinding);

	MonoObject *InvokeFunc(string funcName, MonoClass *pClass, MonoObject *pInstance = NULL, void **args = NULL);

	// Uses native library if pImage is NULL
	MonoObject *InvokeFunc(string funcName, string _className, string _nameSpace, MonoImage *pImage = NULL, MonoObject *pInstance = NULL, void **args = NULL);

	MonoObject *CreateClassInstance(MonoClass *pClass) { return mono_object_new (m_pMonoDomain, pClass); }
	MonoImage *GetNativeLibraryImage() { return mono_assembly_get_image(m_pCryEngineAssembly); }

	MonoAssembly *LoadAssembly(const char *fullPath);

	MonoString *ToMonoString(string text) { return mono_string_new(m_pMonoDomain, text); }

	CFGPluginManager *GetFGPluginManager() const { return m_pFGPluginManager; }

	bool IsFullyInitialized() const { return m_bInitialized; }
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

	CFGPluginManager					*m_pFGPluginManager;
	bool m_bInitialized;
};

extern CMono* g_pMono;

#endif //__MONO_H__