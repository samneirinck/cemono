#ifndef __MONO_STRING_H__
#define __MONO_STRING_H__

#define ToCryString(monoString) mono_string_to_utf8(monoString)
#define ToMonoString(string) mono_string_new(mono_domain_get(), string)

#endif //__MONO_STRING_H__