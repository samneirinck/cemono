/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// Common CryMono types and methods.
//////////////////////////////////////////////////////////////////////////
// 11/01/2012 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __MONO_COMMON_H__
#define __MONO_COMMON_H__

namespace mono 
{
	class _string; typedef _string* string; 
	class _array; typedef _array* array;
};

#include <IMonoScriptSystem.h>
#include <IMonoConverter.h>

inline const char *ToCryString(mono::string monoString)
{
	return gEnv->pMonoScriptSystem->GetConverter()->ToString(monoString);
}

inline mono::string ToMonoString(const char *cryString)
{
	return gEnv->pMonoScriptSystem->GetConverter()->ToMonoString(cryString);
}

// Wrapper 'helpers' used to easily convert native mono objects to IMonoArray's, strings etc.
namespace mono
{
	class _string
	{
	public:
		operator const char *() const
		{
			return ToCryString(const_cast<_string *>(this));
		}

		operator CryStringT<char>() const
		{
			return (CryStringT<char>)ToCryString(const_cast<_string *>(this));
		}
	};

	class _array
	{
		operator IMonoArray *() const
		{
			return gEnv->pMonoScriptSystem->GetConverter()->ToArray(const_cast<_array *>(this));
		}
	};

	typedef _string* string;
	typedef _array* array;
};

#endif //__MONO_COMMON_H__