/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// Wrapper for the MonoArray for less intensively ugly code and
// better workflow.
//////////////////////////////////////////////////////////////////////////
// 17/12/2011 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __MONO_ASSEMBLY_H__
#define __MONO_ASSEMBLY_H__

#include <IMonoAssembly.h>

struct IMonoScript;
struct IMonoArray;

class CScriptAssembly : public IMonoAssembly
{
public:
	CScriptAssembly(MonoImage *pImage) : m_pImage(pImage), m_pAssembly(NULL) {}
	virtual ~CScriptAssembly();

	// IMonoAssembly
	virtual void Release() override { delete this; }

	virtual IMonoClass *GetClass(const char *className, const char *nameSpace = "CryEngine") override;
	// ~IMonoAssembly

	MonoImage *GetImage() const { return m_pImage; }

	static const char *Relocate(const char *originalAssemblyPath);

private:
	MonoAssembly *m_pAssembly;

	MonoImage *m_pImage;
};

#endif //__MONO_ASSEMBLY_H__