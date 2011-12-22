#ifndef __MONO_ASSEMBLY_H__
#define __MONO_ASSEMBLY_H__

#include <IMonoAssembly.h>

struct IMonoClass;
struct IMonoArray;

class CMonoAssembly : public IMonoAssembly
{
public:
	CMonoAssembly(MonoDomain *pDomain, const char *assemblyPath);
	virtual ~CMonoAssembly() {}

	virtual IMonoClass *InstantiateClass(const char *nameSpace, const char *className, IMonoArray *pConstructorArguments = NULL) override;

	MonoImage *GetImage() const { return m_pImage; }

private:
	MonoClass *GetClassFromName(const char* nameSpace, const char* className);

protected:
	const char *m_assemblyPath;
	MonoAssembly *m_pAssembly;

	MonoImage *m_pImage;
};

#endif //__MONO_ASSEMBLY_H__