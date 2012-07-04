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
	CScriptAssembly(MonoImage *pImage, const char *path, bool nativeAssembly = true);
	virtual ~CScriptAssembly();

	CScriptClass *TryGetClass(MonoClass *pClass);

	static CScriptAssembly *TryGetAssembly(MonoImage *pImage);
	static CScriptClass *TryGetClassFromRegistry(MonoClass *pClass);

	// IMonoAssembly
	virtual IMonoClass *GetClass(const char *className, const char *nameSpace = "CryEngine") override;

	virtual const char *GetName() override { return mono_image_get_name(m_pImage); }
	virtual const char *GetPath() override { return m_path.c_str(); }

	virtual bool IsNative() override { return m_bNative; }
	// ~IMonoAssembly

	void SetImage(MonoImage *pImage) { m_pImage = pImage; }
	MonoImage *GetImage() const { return m_pImage; }

	void SetPath(const char *path) { m_path = string(path); }

private:
	string m_path;
	MonoImage *m_pImage;

	bool m_bNative;

	TClassMap m_classRegistry;
};

#endif //__MONO_ASSEMBLY_H__