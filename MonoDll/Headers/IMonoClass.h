/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// IMonoClass interface for external projects, i.e. CryGame.
//////////////////////////////////////////////////////////////////////////
// 18/12/2011 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __I_MONO_CLASS__
#define __I_MONO_CLASS__

#include <IMonoScriptSystem.h>
#include <IMonoScriptManager.h>

#include <IMonoArray.h>

struct IMonoObject;

/// <summary>
/// Reference to a Mono class, used to call static methods and etc.
/// </summary>
/// <example>
/// IMonoClass *pCryNetwork = gEnv->pMonoScriptSystem->GetCustomClass("CryNetwork", "CryEngine");
/// </example>
struct IMonoClass
{
public:
	/// <summary>
	/// Deletes the class, its instance and used objects.
	/// </summary>
	virtual void Release() = 0;

	/// <summary>
	/// Gets the class name.
	/// </summary>
	virtual const char *GetName() = 0;

	/// <summary>
	/// Gets the script id, returns -1 if this is not a proper script. (i.e. if this was constructed via C++)
	/// </summary>
	virtual int GetScriptId() = 0;

	/// <summary>
	/// Instantiates the class, if not already instantiated.
	/// </summary>
	virtual void Instantiate(IMonoArray *pConstructorParams = NULL) = 0;

	/// <summary>
	/// Invokes a method on the class.
	/// </summary>
	/// <example>
	/// CallMethod("InitializeNetwork", pParameters, true");
	/// </example>
	virtual IMonoObject *CallMethod(const char *methodName, IMonoArray *params = NULL, bool _static = false) = 0;
	/// <summary>
	/// Gets a property within the class as an IMonoObject.
	/// </summary>
	/// <example>
	/// [C#] public float X { get; set; }
	/// [C++] float X = GetProperty("X")->Unbox<float>();
	/// </example>
	virtual IMonoObject *GetProperty(const char *propertyName) = 0;
	/// <summary>
	/// Sets a property within the class.
	/// </summary>
	virtual void SetProperty(const char *propertyName, IMonoObject *pNewValue) = 0;
	/// <summary>
	/// Gets a field within the class.
	/// </summary>
	virtual IMonoObject *GetField(const char *fieldName) = 0;
	/// <summary>
	/// Sets a field within the class.
	/// </summary>
	virtual void SetField(const char *fieldName, IMonoObject *pNewValue) = 0;

	template <typename TResult>
	static TResult CallMethod(int scriptId, const char *funcName, IMonoArray *pArgs = NULL, bool releaseArgs = false)
	{
		TResult result;

		if(IMonoClass *pClass = gEnv->pMonoScriptSystem->GetScriptManager()->GetScriptById(scriptId))
		{
			if(IMonoObject *pResult = pClass->CallMethod(funcName, pArgs))
			{
				result = pResult->Unbox<TResult>();

				SAFE_RELEASE(pResult);
				if(releaseArgs)
					SAFE_RELEASE(pArgs);
			}
		}

		return result;
	}

	template <>
	static void CallMethod(int scriptId, const char *funcName, IMonoArray *pArgs, bool releaseArgs)
	{
		if(IMonoClass *pClass = gEnv->pMonoScriptSystem->GetScriptManager()->GetScriptById(scriptId))
			pClass->CallMethod(funcName, pArgs);

		SAFE_RELEASE(pArgs);
	}
};

#endif //__I_MONO_CLASS__