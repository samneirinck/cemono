#ifndef __I_MONO_CLASS__
#define __I_MONO_CLASS__

struct IMonoObject;
struct IMonoArray;

struct IMonoClass
{
public:
	virtual IMonoObject *CallMethod(const char *methodName, IMonoArray *params = NULL, bool _static = false) = 0;
};

#endif //__I_MONO_CLASS__