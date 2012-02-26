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
#define __MONO_TESTER_H__

#include <IMonoScriptBind.h>

#include <MonoCommon.h>

struct IMonoScriptSystem;

struct IMonoAssembly;
struct IMonoArray;

class CTester : public IMonoScriptBind
{
public:
	CTester();
	~CTester() {}

	void Benchmark();
	void CommenceTesting();

	void TestStaticMethods(IMonoAssembly *pCryBrary);
	void TestInstantiatedMethods(IMonoAssembly *pCryBrary);

protected:
	// IMonoScriptBind
	virtual const char *GetNamespace() override { return "CryEngine.Utils"; }
	virtual const char *GetClassName() { return "Tester"; }
	// ~IMonoScriptBind

	// Scriptbind methods
	static void TestScriptBind(mono::string, int, mono::array);
	// ~Scriptbind methods

	static IMonoArray *GetTestParameters();

	int m_refs;
};

#endif //__MONO_TESTER_H__