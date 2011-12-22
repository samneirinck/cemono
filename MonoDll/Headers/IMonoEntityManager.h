#ifndef __I_MONO_ENTITY_MANAGER_H__
#define __I_MONO_ENTITY_MANAGER_H__

struct IMonoEntityManager
{
	virtual int GetScriptId(EntityId entityId) = 0;
};

#endif //__I_MONO_ENTITY_MANAGER_H__