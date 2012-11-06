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

#include "MonoObject.h"

#include <IMonoAssembly.h>

struct IMonoScript;
struct IMonoArray;

class CScriptClass;
class CScriptDomain;

class CScriptAssembly 
	: public CScriptObject
	, public IMonoAssembly
{
	typedef std::map<CScriptClass *, MonoClass *> TClassMap;
public:
	CScriptAssembly(CScriptDomain *pDomain, MonoImage *pImage, const char *path, bool nativeAssembly = true);
	virtual ~CScriptAssembly();

	CScriptClass *TryGetClass(MonoClass *pClass);

	static CScriptClass *TryGetClassFromRegistry(MonoClass *pClass);

	// IMonoAssembly
	virtual IMonoClass *GetClass(const char *className, const char *nameSpace = "CryEngine") override;

	virtual const char *GetName() override { return mono_image_get_name((MonoImage *)m_pObject); }
	virtual const char *GetPath() override { return m_path.c_str(); }

	virtual bool IsNative() override { return m_bNative; }

	virtual IMonoDomain *GetDomain() override { return (IMonoDomain *)m_pDomain; }
	// ~IMonoAssembly

	// IMonoObject
	virtual void Release(bool triggerGC = true) override;

	virtual EMonoAnyType GetType() override { return eMonoAnyType_Assembly; }
	virtual MonoAnyValue GetAnyValue() override { return MonoAnyValue(); }

	virtual mono::object GetManagedObject() override { return (mono::object)mono_assembly_get_object(mono_domain_get(), mono_image_get_assembly((MonoImage *)m_pObject)); }

	virtual IMonoClass *GetClass() override { return CScriptObject::GetClass(); }

	virtual void *UnboxObject() override { return CScriptObject::UnboxObject(); }
	// ~IMonoObject

	/// <summary>
	/// Called when a IMonoClass created from this assembly is released.
	/// </summary>
	void OnClassReleased(CScriptClass *pClass);

	void SetImage(MonoImage *pImage) { m_pObject = (MonoObject *)pImage; }
	MonoImage *GetImage() const { return (MonoImage *)m_pObject; }

	void SetPath(const char *path) { m_path = string(path); }

private:
	string m_path;
	bool m_bNative;

	TClassMap m_classRegistry;
	CScriptDomain *m_pDomain;
};

#endif //__MONO_ASSEMBLY_H__