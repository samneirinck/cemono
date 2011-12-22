#include "StdAfx.h"
#include "MonoTester.h"

#include "MonoScriptSystem.h"

#include "MonoClass.h"
#include "MonoAssembly.h"
#include "MonoObject.h"
#include "MonoArray.h"

CMonoTester::CMonoTester()
{
	CMonoScriptSystem *pScriptSystem = static_cast<CMonoScriptSystem *>(gEnv->pMonoScriptSystem);

	// Scriptbinds
	REGISTER_METHOD(TestScriptBind);

	pScriptSystem->RegisterScriptBind(this);
	// ~Scriptbinds

	CryLogAlways("-----------------------------");
	CryLogAlways("Initializing mono feature tester");
	CryLogAlways("-----");
	CryLogAlways("Commencing static method test");
	TestStaticMethods(pScriptSystem);
	CryLogAlways("-----");
	CryLogAlways("Commencing instantiated class & method test");
	TestInstantiatedMethods(pScriptSystem);
	//CryLogAlways("-----");
	//CryLogAlways("Commencing C# / Lua benchmark");
	//Benchmark();
	CryLogAlways("-----------------------------");
}

bool CMonoTester::TestStaticMethods(CMonoScriptSystem *pScriptSystem)
{
	CryLogAlways("---");
	CryLogAlways("Attempting to get static class");
	IMonoClass *pTesterStatic = pScriptSystem->GetCustomClass("Tester", "CryMono", false);
	if(pTesterStatic)
		CryLogAlways("success.");

	CryLogAlways("---");
	CryLogAlways("Attempting to invoke static method without parameters");
	if(IMonoObject *pResult = pTesterStatic->CallMethod("StaticMethodNoParameters", NULL, true))
	{
		if(pResult->Unbox<float>()==13.37f)
			CryLogAlways("success.");
	}

	CryLogAlways("---");
	CryLogAlways("Attempting to invoke static method with parameters");
	pTesterStatic->CallMethod("StaticMethodWithParameters", GetTestParameters(), true);

	SAFE_DELETE(pTesterStatic);

	return true;
}

bool CMonoTester::TestInstantiatedMethods(CMonoScriptSystem *pScriptSystem)
{
	CryLogAlways("---");
	CryLogAlways("Attempting to instantiate class");
	IMonoClass *pTesterClassStandardConstructor = pScriptSystem->InstantiateClass("Tester", "CryMono");
	if(pTesterClassStandardConstructor)
		CryLogAlways("success.");

	SAFE_DELETE(pTesterClassStandardConstructor);

	CryLogAlways("---");
	CryLogAlways("Attempting to instantiate class with complex constructor");
	IMonoClass *pTesterClassComplexConstructor = pScriptSystem->InstantiateClass("Tester", "CryMono", GetTestParameters());
	if(pTesterClassComplexConstructor)
		CryLogAlways("success.");

	CryLogAlways("---");
	CryLogAlways("Attempting to invoke method without parameters");
	if(IMonoObject *pResult = pTesterClassComplexConstructor->CallMethod("MethodNoParameters"))
	{
		if(pResult->Unbox<float>()==13.37f)
			CryLogAlways("success.");
	}

	CryLogAlways("---");
	CryLogAlways("Attempting to invoke method with parameters");
	pTesterClassComplexConstructor->CallMethod("MethodWithParameters", GetTestParameters());

	SAFE_DELETE(pTesterClassComplexConstructor);

	return true;
}

void CMonoTester::TestScriptBind(MonoString *testString, int testInt, MonoArray *testArray)
{
	if(!strcmp(ToCryString(testString), "TestStringContents") && testInt == 1337 && mono_array_length(testArray)==2)
	{
		CMonoArray *pTestArray = new CMonoArray(testArray);
		const char *unboxedString = pTestArray->GetItemString(0);
		int unboxedInt = pTestArray->GetItemUnboxed<int>(1);

		if(/*!strcmp(unboxedString, "TestStringContents") &&*/ unboxedInt == 1337)
			CryLogAlways("success.");
	}
}

IMonoArray *CMonoTester::GetTestParameters()
{
	const char *testString = "TestStringContents";
	int testInt = 1337;

	CMonoArray *pParameters = new CMonoArray(3);
	pParameters->InsertString(testString);
	pParameters->InsertObject(CreateMonoObject<int>(testInt));

	CMonoArray subArray(2);
	subArray.InsertString(testString);
	subArray.InsertObject(CreateMonoObject<int>(testInt));
	pParameters->InsertArray(&subArray);

	return pParameters;
}

void CMonoTester::Benchmark()
{
	/*
	// Lua
	IScriptTable *pBenchmarkScript = NULL;

	clock_t start = clock();
	if(gEnv->pScriptSystem->ExecuteFile("D:\\Dev\\INK\\ProjectInk\\Game\\Scripts\\Tests\\Benchmark.lua", true, false, pBenchmarkScript));
	long luaExecutionTime = (clock() - start) / CLOCKS_PER_SEC * 1000;

	//start = clock();

	long monoExecutionTime = 0;//(clock() - start) / CLOCKS_PER_SEC * 1000;

	CryLogAlways("Benchmark done; Lua executed the script in %f ms and C# executed the script in %f ms", luaExecutionTime, monoExecutionTime);*/
}