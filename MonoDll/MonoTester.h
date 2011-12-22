#ifndef __MONO_TESTER_H__
#define __MONO_TESTER_H__

#include "Headers\IMonoScriptBind.h"

class CMonoScriptSystem;
class CMonoAssembly;

struct IMonoArray;

class CMonoTester : public IMonoScriptBind
{
public:
	CMonoTester();
	~CMonoTester() {}

	bool TestStaticMethods(CMonoScriptSystem *pScriptSystem);
	bool TestInstantiatedMethods(CMonoScriptSystem *pScriptSystem);

	void Benchmark();

protected:

	// IMonoScriptBind
	virtual const char* GetNamespace() override { return "CryMono"; }
	virtual const char* GetClassName() override { return "Tester"; }
	// ~IMonoScriptBind

	// Scriptbind methods
	static void TestScriptBind(MonoString *testString, int testInt, MonoArray *testArray);
	// ~Scriptbind methods

	static IMonoArray *GetTestParameters();
};

#endif //__MONO_TESTER_H__