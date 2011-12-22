#ifndef __I_MONO_SCRIPT_SYSTEM_H__
#define __I_MONO_SCRIPT_SYSTEM_H__

#include <CryExtension/ICryUnknown.h>

struct IMonoScriptBind;
struct IMonoScript;
struct IMonoObject;

struct IMonoEntityManager;

enum EMonoScriptType
{
	EMonoScriptType_NULL = 0,

	EMonoScriptType_GameRules,
	EMonoScriptType_FlowNode,
	EMonoScriptType_Entity
};

enum MonoAnyType
{
	MONOTYPE_BOOL = 0,

	MONOTYPE_INT,
	MONOTYPE_UINT,
	MONOTYPE_FLOAT,
	MONOTYPE_VEC3,
	MONOTYPE_ANG3,

	MONOTYPE_STRING,

	MONOTYPE_NULL,
};

struct SMonoAnyValue
{
	SMonoAnyValue(bool value) : type(MONOTYPE_BOOL) { b = value; }
	SMonoAnyValue(int value) : type(MONOTYPE_INT) { number = (float)value; }
	SMonoAnyValue(unsigned int value) : type(MONOTYPE_UINT) { number = (float)value; }
	SMonoAnyValue(float value) : type(MONOTYPE_FLOAT) { number = value; }
	SMonoAnyValue(const char *value) : type(MONOTYPE_STRING) { str = value; }
	SMonoAnyValue(string value) : type(MONOTYPE_STRING) { str = value.c_str(); }
	SMonoAnyValue(Vec3 value) : type(MONOTYPE_VEC3) { number = value.x; num2 = value.y; num3 = value.z; }
	SMonoAnyValue(Ang3 value) : type(MONOTYPE_ANG3) { number = value.x; num2 = value.y; num3 = value.z; }

	MonoAnyType type;
	union
	{
		bool b;
		float number;
		const char *str;
		float num2;
		float num3;
	};
};

struct IMonoScriptSystem : public ICryUnknown
{
	CRYINTERFACE_DECLARE(IMonoScriptSystem, 0x86169744ce38420f, 0x9768a98386be991f)

	virtual bool Init() = 0 ;
	virtual void Release() = 0;

	// Updates the system, once per frame.
	virtual void Update() = 0;

	virtual IMonoEntityManager *GetEntityManager() const = 0;

	virtual void RegisterScriptBind(IMonoScriptBind *pScriptBind) = 0;

	virtual int InstantiateScript(EMonoScriptType scriptType, const char* scriptName) = 0;
	virtual IMonoScript *GetScriptById(int id) = 0;

	virtual void BeginCall(int scriptId, const char *funcName, int numArgs = 0) = 0;
	virtual void PushFuncParamAny(const SMonoAnyValue &any) = 0;
	template <class T> void PushFuncParam(const T &value) { PushFuncParamAny(value); }
	virtual IMonoObject *EndCall() = 0;

	typedef void *(*TEntryFunction)(ISystem* pSystem);
};

static IMonoObject *CallMonoScript(int scriptId, const char *funcName)
{
	IMonoScriptSystem *pScriptSystem = gEnv->pMonoScriptSystem;
	pScriptSystem->BeginCall(scriptId, funcName);
	
	return pScriptSystem->EndCall();
};
template<typename P1> 
static IMonoObject *CallMonoScript(int scriptId, const char *funcName, const P1 &p1)
{
	if(scriptId==-1)
		return NULL;

	IMonoScriptSystem *pScriptSystem = gEnv->pMonoScriptSystem;
	pScriptSystem->BeginCall(scriptId, funcName, 1);
	pScriptSystem->PushFuncParam(p1);
	
	return pScriptSystem->EndCall();
};
template<typename P1, typename P2> 
static IMonoObject *CallMonoScript(int scriptId, const char *funcName, const P1 &p1, const P2 &p2)
{
	if(scriptId==-1)
		return NULL;

	IMonoScriptSystem *pScriptSystem = gEnv->pMonoScriptSystem;
	pScriptSystem->BeginCall(scriptId, funcName, 2);
	pScriptSystem->PushFuncParam(p1); pScriptSystem->PushFuncParam(p2);
	
	return pScriptSystem->EndCall();
};
template<typename P1, typename P2, typename P3> 
static IMonoObject *CallMonoScript(int scriptId, const char *funcName, const P1 &p1, const P2 &p2, const P3 &p3)
{
	if(scriptId==-1)
		return NULL;

	IMonoScriptSystem *pScriptSystem = gEnv->pMonoScriptSystem;
	pScriptSystem->BeginCall(scriptId, funcName, 3);
	pScriptSystem->PushFuncParam(p1); pScriptSystem->PushFuncParam(p2); pScriptSystem->PushFuncParam(p3);
	
	return pScriptSystem->EndCall();
};
template<typename P1, typename P2, typename P3, typename P4> 
static IMonoObject *CallMonoScript(int scriptId, const char *funcName, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4)
{
	if(scriptId==-1)
		return NULL;

	IMonoScriptSystem *pScriptSystem = gEnv->pMonoScriptSystem;
	pScriptSystem->BeginCall(scriptId, funcName, 4);
	pScriptSystem->PushFuncParam(p1); pScriptSystem->PushFuncParam(p2); pScriptSystem->PushFuncParam(p3); pScriptSystem->PushFuncParam(p4);
	
	return pScriptSystem->EndCall();
};
template<typename P1, typename P2, typename P3, typename P4, typename P5> 
static IMonoObject *CallMonoScript(int scriptId, const char *funcName, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4, const P5 &p5)
{
	if(scriptId==-1)
		return NULL;

	IMonoScriptSystem *pScriptSystem = gEnv->pMonoScriptSystem;
	pScriptSystem->BeginCall(scriptId, funcName, 5);
	pScriptSystem->PushFuncParam(p1); pScriptSystem->PushFuncParam(p2); pScriptSystem->PushFuncParam(p3); pScriptSystem->PushFuncParam(p4); pScriptSystem->PushFuncParam(p5);
	
	return pScriptSystem->EndCall();
};
template<typename P1, typename P2, typename P3, typename P4, typename P5, typename P6> 
static IMonoObject *CallMonoScript(int scriptId, const char *funcName, const P1 &p1, const P2 &p2, const P3 &p3, const P4 &p4, const P5 &p5, const P6 &p6)
{
	if(scriptId==-1)
		return NULL;

	IMonoScriptSystem *pScriptSystem = gEnv->pMonoScriptSystem;
	pScriptSystem->BeginCall(scriptId, funcName, 6);
	pScriptSystem->PushFuncParam(p1); pScriptSystem->PushFuncParam(p2); pScriptSystem->PushFuncParam(p3); pScriptSystem->PushFuncParam(p4); pScriptSystem->PushFuncParam(p5); pScriptSystem->PushFuncParam(p6);

	return pScriptSystem->EndCall();
};

typedef boost::shared_ptr<IMonoScriptSystem> IMonoPtr;

#endif //__I_MONO_SCRIPT_SYSTEM_H__