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

class CScriptClass;

class CScriptAssembly : public IMonoAssembly
{
	typedef std::map<CScriptClass *, MonoClass *> TClassMap;
public:
	CScriptAssembly(MonoImage *pImage, const char *path);
	virtual ~CScriptAssembly();

	CScriptClass *TryGetClass(MonoClass *pClass);

	static CScriptAssembly *TryGetAssembly(MonoImage *pImage);
	static CScriptClass *TryGetClassFromRegistry(MonoClass *pClass);

	// IMonoAssembly
	virtual void Release() override { delete this; }

	virtual IMonoClass *GetClass(const char *className, const char *nameSpace = "CryEngine") override;

	virtual const char *GetPath() override { return m_path; }
	// ~IMonoAssembly

	void SetImage(MonoImage *pImage) { m_pImage = pImage; }
	MonoImage *GetImage() const { return m_pImage; }

	// Keep all assemblies we get around, for class registries etc.
	static std::vector<CScriptAssembly *> m_assemblies;

private:
	const char *m_path;
	MonoImage *m_pImage;

	TClassMap m_classRegistry;
};

#endif //__MONO_ASSEMBLY_H__