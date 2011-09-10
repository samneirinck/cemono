#pragma once

#include "stdafx.h"
#include <mono/jit/jit.h>


class CCemonoString
{
public:
	static const char* ToString(MonoString* monoString)
	{
		return mono_string_to_utf8(monoString);
	}

	static MonoString* ToMonoString(const char* string)
	{
		return mono_string_new(mono_domain_get(), string);
	}
};

