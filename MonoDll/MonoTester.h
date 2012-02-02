/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// MonoTester, used at startup to determine if all Mono subsystems
// are functional.
//////////////////////////////////////////////////////////////////////////
// 18/12/2011 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __MONO_TESTER_H__
#define __MONO_TESTER_H

#include "IMonoAutoRegScriptBind.h"

#include <MonoCommon.h>

struct IMonoScriptSystem;

struct IMonoAssembly;
struct IMonoArray;

class CMonoTester : public IMonoAutoRegScriptBind
{
public:
	CMonoTester();
	~CMonoTester() {}

	void Benchmark();
	void CommenceTesting();

	void TestStaticMethods(IMonoAssembly *pCryBrary);
	void TestInstantiatedMethods(IMonoAssembly *pCryBrary);

	// IMonoScriptBind
	virtual void AddRef() override { m_refs++; }
	virtual void Release() override { if( 0 >= --m_refs) delete this; }

protected:
	virtual const char* GetNamespace() override { return "CryEngine.Utils"; }
	virtual const char* GetClassName() override { return "Tester"; }
	// ~IMonoScriptBind

	// Scriptbind methods
	MonoMethod(void, TestScriptBind, mono::string, int, mono::array);
	// ~Scriptbind methods

	static IMonoArray *GetTestParameters();

	int m_refs;
};

#endif //__MONO_TESTER_H__