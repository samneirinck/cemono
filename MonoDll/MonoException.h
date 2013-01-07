#ifndef __MONO_EXCEPTION_H__
#define __MONO_EXCEPTION_H__

#include <IMonoException.h>

class CScriptException : public IMonoException
{
public:
	CScriptException(MonoException *pException) : m_pException(pException) { CRY_ASSERT(pException); }
	~CScriptException() { m_pException = nullptr; }

	// IMonoException
	virtual void Throw() { mono_raise_exception(m_pException); }
	// ~IMonoException

	MonoException *m_pException;
};

#endif // __MONO_EXCEPTION_H__