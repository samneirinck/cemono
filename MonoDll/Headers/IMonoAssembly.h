/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// IMonoAssembly interface for external projects, i.e. CryGame.
//////////////////////////////////////////////////////////////////////////
// 18/12/2011 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
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