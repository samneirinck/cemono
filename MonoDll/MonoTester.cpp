#include "StdAfx.h"
#include "MonoTester.h"

#include "MonoScriptSystem.h"

#include "MonoClass.h"
#include "MonoAssembly.h"
#include "MonoObject.h"
#include "MonoArray.h"

CTester::CTester()
	: m_refs(0)
{
	IMonoScriptSystem *pScriptSystem = gEnv->pMonoScriptSystem;

	REGISTER_METHOD(TestScriptBind);
}

void CTester::CommenceTesting()
{
	IMonoScriptSystem *pScriptSystem = gEnv->pMonoScriptSystem;

	IMonoAssembly *pCryBraryAssembly = gEnv->pMonoScriptSystem->GetCryBraryAssembly();
	TestStaticMethods(pCryBraryAssembly);
	TestInstantiatedMethods(pCryBraryAssembly);
}

void CTester::TestStaticMethods(IMonoAssembly *pCryBrary)
{
	IMonoClass *pTesterStatic = pCryBrary->GetCustomClass("Tester", "CryEngine.Utils");
	if(!pTesterStatic)
	{
		CryLogAlways("Mono feature test failed! Failed to obtain static tester class.");

		return;
	}

	if(pTesterStatic->CallMethod("StaticMethodNoParameters", NULL, true)->Unbox<float>()!=13.37f)
	{
		CryLogAlways("Mono feature test failed! Static method result was invalid.");

		return;
	}

	IMonoArray *pTestParams = GetTestParameters();
	pTesterStatic->CallMethod("StaticMethodWithParameters", pTestParams, true);
	SAFE_RELEASE(pTestParams);

	SAFE_RELEASE(pTesterStatic);
}

void CTester::TestInstantiatedMethods(IMonoAssembly *pCryBrary)
{
	IMonoClass *pTesterClassStandardConstructor = pCryBrary->InstantiateClass("Tester", "CryEngine.Utils");
	if(!pTesterClassStandardConstructor)
	{
		CryLogAlways("Mono feature test failed! Failed to obtain tester class.");

		return;
	}

	SAFE_RELEASE(pTesterClassStandardConstructor);

	IMonoArray *pTestParams = GetTestParameters();
	IMonoClass *pTesterClassComplexConstructor = pCryBrary->InstantiateClass("Tester", "CryEngine.Utils", pTestParams);
	if(!pTesterClassComplexConstructor)
	{
		CryLogAlways("Mono feature test failed! Failed to instantiate tester class.");

		return;
	}

	if(pTesterClassComplexConstructor->CallMethod("MethodNoParameters")->Unbox<float>()!=13.37f)
	{
		CryLogAlways("Mono feature test failed! Method results were invalid.");

		return;
	}


	pTesterClassComplexConstructor->CallMethod("MethodWithParameters", pTestParams);

	SAFE_RELEASE(pTesterClassComplexConstructor);
	SAFE_RELEASE(pTestParams);
}

void CTester::TestScriptBind(mono::string testString, int testInt, mono::array testArray)
{
	CScriptArray *pTestArray = new CScriptArray(testArray);

	if(!strcmp(ToCryString(testString), "TestStringContents") && testInt == 1337 && pTestArray->GetSize()==2)
	{
		const char *unboxedString = pTestArray->GetItemString(0);
		int unboxedInt = pTestArray->GetItemUnboxed<int>(1);

		if(/*!strcmp(unboxedString, "TestStringContents") &&*/ unboxedInt != 1337)
		{
			CryLogAlways("Mono feature test failed! Scriptbind result was invalid.");

			return;
		}
	}
	else
	{
		CryLogAlways("Mono feature test failed! Scriptbind parameters were invalid.");

		return;
	}
}

IMonoArray *CTester::GetTestParameters()
{
	const char *testString = "TestStringContents";
	int testInt = 1337;

	IMonoArray *pParameters = CreateMonoArray(3);
	pParameters->Insert(testString);
	pParameters->Insert(testInt);

	IMonoArray *pSubArray = CreateMonoArray(2);
	pSubArray->Insert(testString);
	pSubArray->Insert(testInt);
	pParameters->Insert(pSubArray);

	return pParameters;
}

void CTester::Benchmark()
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