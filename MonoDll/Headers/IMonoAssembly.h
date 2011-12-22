#ifndef __I_MONO_ASSEMBLY__
#define __I_MONO_ASSEMBLY__

struct IMonoClass;
struct IMonoArray;

struct IMonoAssembly
{
public:
	virtual IMonoClass *InstantiateClass(const char *nameSpace, const char *className, IMonoArray *pConstructorArguments = NULL) = 0;
};

#endif //__I_MONO_ASSEMBLY__`	