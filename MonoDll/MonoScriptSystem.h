#ifndef __MONO_H__
#define __MONO_H__

#include <IMonoScriptSystem.h>
#include <IMonoScript.h>

#include <CryExtension/Impl/ClassWeaver.h>

struct IMonoClass;
struct IMonoAssembly;
class CMonoTester;
class CMonoScript;

class CEntityManager;
class CMonoCallbackHandler;

struct SMonoCallParams
{
	SMonoCallParams() {}
	SMonoCallParams(int id, const char *func, IMonoArray *array) : scriptId(id), funcName(func), pArray(array) {}
	SMonoCallParams(int id, const char *func) : scriptId(id), funcName(func), pArray(NULL) {}

	~SMonoCallParams() {}

	int scriptId;
	const char *funcName;
	IMonoArray *pArray;
};

class CMonoScriptSystem : public IMonoScriptSystem
{
	CRYINTERFACE_BEGIN()
	CRYINTERFACE_ADD(IMonoScriptSystem)
	CRYINTERFACE_END()
	
	CRYGENERATE_SINGLETONCLASS(CMonoScriptSystem, "CryMono", 0xc37b8ad5d62f47de, 0xa8debe525ff0fc8a)

	typedef std::vector<CMonoScript *> TScripts;

public:
	// IMonoScriptSystem
	virtual bool Init() override;
	virtual void Release() override { delete this; }

	virtual void Update() override;

	virtual IMonoEntityManager *GetEntityManager() const;

	virtual void RegisterScriptBind(IMonoScriptBind *pScriptBind) override;

	virtual int InstantiateScript(EMonoScriptType scriptType, const char* scriptName) override;
	virtual IMonoScript *GetScriptById(int id) override;

	virtual void BeginCall(int scriptId, const char *funcName, int numArgs = 0) override;
	virtual void PushFuncParamAny(const SMonoAnyValue &any) override;
	virtual IMonoObject *EndCall() override;
	// ~IMonoScriptSystem

	// Instantiates a class in the Manager assembly
	IMonoClass *InstantiateClass(const char *className, const char *nameSpace = "CryEngine", IMonoArray *pConstructorParameters = NULL);

	void RemoveScriptInstance(EMonoScriptType scriptType, const char* scriptName);

	IMonoClass *GetManagerClass() const { return m_pManagerClass; }
	IMonoClass *GetCustomClass(const char *className, const char *nameSpace = "CryEngine", bool baseClassAssembly = true /* uses manager assembly if false */);

	CMonoCallbackHandler *GetCallbackHandler() const { return m_pCallbackHandler; }

private:
	bool InitializeDomain();
	void RegisterDefaultBindings();
	bool InitializeBaseClassLibraries();
	bool InitializeManager();

	MonoDomain *m_pMonoDomain;
	IMonoAssembly *m_pBclAssembly;
	IMonoAssembly *m_pManagerAssembly;
	IMonoClass *m_pManagerClass;

	CEntityManager *m_pEntityManager;
	CMonoCallbackHandler *m_pCallbackHandler;

#ifndef _RELEASE
	CMonoTester *m_pTester;
#endif

	bool m_bDebugging;

	TScripts m_scripts;

	SMonoCallParams m_callParams;
};
#endif //__MONO_H__