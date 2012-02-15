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

struct IMonoClass;
struct IMonoArray;

class CMonoAssembly : public IMonoAssembly
{
public:
	CMonoAssembly(const char *assemblyPath);
	virtual ~CMonoAssembly();

	// IMonoAssembly
	virtual void Release() override { delete this; }

	virtual IMonoClass *InstantiateClass(const char *className, const char *nameSpace = "CryEngine", IMonoArray *pConstructorArguments = NULL) override;
	virtual IMonoClass *GetCustomClass(const char *className, const char *nameSpace = "CryEngine") override;
	// ~IMonoAssembly

	MonoImage *GetImage() const { return m_pImage; }

private:
	MonoClass *GetClassFromName(const char* nameSpace, const char* className);

protected:
	const char *m_assemblyPath;
	MonoAssembly *m_pAssembly;

	MonoImage *m_pImage;
};

#endif //__MONO_ASSEMBLY_H__