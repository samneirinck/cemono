#include "StdAfx.h"
#include "MonoEntity.h"

std::vector<CMonoEntity*> CMonoEntity::m_monoEntities;

CMonoEntity::CMonoEntity()
{
}



CMonoEntity::CMonoEntity(MonoClass *pClass, MonoImage *pImage)
{
	m_pClass = pClass;
	m_pImage = pImage;

	// Construct obj
	m_pMonoObject = mono_object_new(mono_domain_get(), pClass);
	if (!m_pMonoObject)
	{
		CryError("Failed to create mono object :(");
	}

	mono_runtime_object_init(m_pMonoObject);

}

CMonoEntity::~CMonoEntity()
{
	m_monoEntities.erase(std::remove(m_monoEntities.begin(), m_monoEntities.end(), this), m_monoEntities.end());
}

bool CMonoEntity::Init(IGameObject *pGameObject)
{
	m_monoEntities.push_back(this);

	SetGameObject(pGameObject);

	if (m_pMonoObject && m_pClass)
	{
		MonoMethodDesc* desc = mono_method_desc_new(":OnInit", false);

		MonoMethod *pMethod = mono_method_desc_search_in_class(desc, m_pClass);

		if (!pMethod)
		{
			// Try virtual method
			MonoAssembly *pAss = g_pMono->GetBclAssembly();

			MonoClass* test = mono_class_from_name(mono_assembly_get_image(pAss), "Cemono", "Entity");
			pMethod = mono_method_desc_search_in_class(desc, test);
			pMethod = mono_object_get_virtual_method(m_pMonoObject, pMethod);
		}

		if (pMethod)
		{
			void *args [1];
			uint val = GetEntityId();
			/* Note we put the address of the value type in the args array */
			args [0] = &val;

			mono_runtime_invoke(pMethod, m_pMonoObject, args, NULL);

		}
	}

	return true;
}
void CMonoEntity::MonoEventFunction(const char* funcName, void** args)
{
	if (m_pMonoObject && m_pClass)
	{
		MonoMethodDesc* desc = mono_method_desc_new(string(":") + funcName, false);
		MonoMethod *pMethod = mono_method_desc_search_in_class(desc, m_pClass);

		if (!pMethod)
		{
			// Try virtual method
			MonoAssembly *pAss = g_pMono->GetBclAssembly();

			MonoClass* test = mono_class_from_name(mono_assembly_get_image(pAss), "Cemono", "Entity");
			pMethod = mono_method_desc_search_in_class(desc, test);
				pMethod = mono_object_get_virtual_method(m_pMonoObject, pMethod);

		}
		if (pMethod)
		{
			mono_runtime_invoke(pMethod, m_pMonoObject, args, NULL);

		}
	}
}

void CMonoEntity::InitClient(int channelId)
{

}
void CMonoEntity::PostInit(IGameObject *pGameObject)
{
	GetGameObject()->EnableUpdateSlot(this, 0);
}
void CMonoEntity::PostInitClient(int channelId)
{

}
void CMonoEntity::Release()
{
}
void CMonoEntity::FullSerialize( TSerialize ser )
{

}
bool CMonoEntity::NetSerialize( TSerialize ser, EEntityAspects aspect, uint8 profile, int flags )
{
	return false;
}
void CMonoEntity::PostSerialize()
{

}
void CMonoEntity::SerializeSpawnInfo( TSerialize ser ) 
{
	gEnv->pTimer->EnableTimer(true);
}
ISerializableInfoPtr CMonoEntity::GetSpawnInfo() 
{
	return 0;
}
void CMonoEntity::Update( SEntityUpdateContext &ctx, int updateSlot)
{

}
void CMonoEntity::PostUpdate(float frameTime ) 
{

}
void CMonoEntity::PostRemoteSpawn()
{

}
void CMonoEntity::HandleEvent( const SGameObjectEvent & evt)
{

}
void CMonoEntity::ProcessEvent(SEntityEvent& evt)
{
	switch (evt.event)
	{
	case ENTITY_EVENT_XFORM:
		void* args[1];
		args[0] = &evt.nParam[0];
		MonoEventFunction("OnXForm", args);
		break;

	case ENTITY_EVENT_TIMER:
		void* args2[2];
		args2[0] = &evt.nParam[0];
		args2[1] = &evt.nParam[1];
		MonoEventFunction("OnTimer", args2);
		break;

	case ENTITY_EVENT_DONE:
		MonoEventFunction("OnDone", NULL);
		break;

	case ENTITY_EVENT_VISIBLITY:
		void* args3[1];
		args3[0] = &evt.nParam[0];
		MonoEventFunction("OnVisibility", args3);

	case ENTITY_EVENT_RESET:
		void *args4[1];
		args4[0] = &evt.nParam[0];
		MonoEventFunction("OnReset", args4);
		break;

	case ENTITY_EVENT_ATTACH:
		void* args5[1];
		args5[0] = &evt.nParam[0];
		MonoEventFunction("OnAttach", args5);
		break;

	case ENTITY_EVENT_DETACH:
		void* args6[1];
		args6[0] = &evt.nParam[0];
		MonoEventFunction("OnDetach", args6);
		break;

	case ENTITY_EVENT_DETACH_THIS:
		MonoEventFunction("OnDetachThis", NULL);
		break;

	case ENTITY_EVENT_HIDE:
		MonoEventFunction("OnHide", NULL);
		break;

	case ENTITY_EVENT_UNHIDE:
		MonoEventFunction("OnUnHide", NULL);
		break;

	case ENTITY_EVENT_ENABLE_PHYSICS:
		void* args99[1];
		args99[0] = &evt.nParam[0];
		MonoEventFunction("OnEnablePhysics", args99);
		break;

	case ENTITY_EVENT_PHYSICS_CHANGE_STATE:
		void* args7[1];
		args[0] = &evt.nParam[0];
		MonoEventFunction("OnPhysicsChangeState", args7);
		break;

	case ENTITY_EVENT_SCRIPT_EVENT:
		void* args8[3];
		args8[0] = &evt.nParam[0];
		args8[1] = &evt.nParam[1];
		args8[2] = &evt.nParam[2];
		MonoEventFunction("OnScriptEvent", args8);
		break;

	case ENTITY_EVENT_ENTERAREA:
		void* args9[3];
		args9[0] = &evt.nParam[0];
		args9[1] = &evt.nParam[1];
		args9[2] = &evt.nParam[2];
		MonoEventFunction("OnEnterArea", args9);
		break;

	case ENTITY_EVENT_LEAVEAREA:
		void* args10[3];
		args10[0] = &evt.nParam[0];
		args10[1] = &evt.nParam[1];
		args10[2] = &evt.nParam[2];
		MonoEventFunction("OnLeaveArea", args10);
		break;

	case ENTITY_EVENT_ENTERNEARAREA:
		void* args11[3];
		args11[0] = &evt.nParam[0];
		args11[1] = &evt.nParam[1];
		args11[2] = &evt.nParam[2];
		MonoEventFunction("OnEnterNearArea", args11);
		break;

	case ENTITY_EVENT_LEAVENEARAREA:
		void* args12[3];
		args12[0] = &evt.nParam[0];
		args12[1] = &evt.nParam[1];
		args12[2] = &evt.nParam[2];
		MonoEventFunction("OnLeaveNearArea", args12);
		break;

	case ENTITY_EVENT_MOVEINSIDEAREA:
		void* args13[3];
		args13[0] = &evt.nParam[0];
		args13[1] = &evt.nParam[1];
		args13[2] = &evt.nParam[2];
		MonoEventFunction("OnMoveInsideArea", args13);
		break;

	case ENTITY_EVENT_MOVENEARAREA:
		void* args14[4];
		args14[0] = &evt.nParam[0];
		args14[1] = &evt.nParam[1];
		args14[2] = &evt.nParam[2];
		args14[3] = &evt.fParam[0];
		MonoEventFunction("OnMoveInsideArea", args14);
		break;

	case ENTITY_EVENT_PHYS_POSTSTEP:
		void* args15[1];
		args15[0] = &evt.fParam[0];
		MonoEventFunction("OnPhysPostStep", args15);
		break;

	case ENTITY_EVENT_PHYS_BREAK:
		MonoEventFunction("OnPhysBreak", NULL);
		break;

	case ENTITY_EVENT_AI_DONE:
		MonoEventFunction("OnAIDone", NULL);
		break;

	case ENTITY_EVENT_SOUND_DONE:
		MonoEventFunction("OnSoundDone", NULL);
		break;

	case ENTITY_EVENT_NOT_SEEN_TIMEOUT:
		MonoEventFunction("OnNotSeenTimeout", NULL);
		break;

	case ENTITY_EVENT_COLLISION:
		MonoEventFunction("OnCollision", NULL);
		break;

	case ENTITY_EVENT_RENDER:
		void* args16[1];
		args16[0] = &evt.nParam[0];
		MonoEventFunction("OnRender", args16);
		break;

	case ENTITY_EVENT_PREPHYSICSUPDATE:
		void* args17[1];
		args17[0] = &evt.fParam[0];
		MonoEventFunction("OnPrePhysicsUpdate", args17);
		break;

	case ENTITY_EVENT_START_LEVEL:
		MonoEventFunction("OnStartLevel", NULL);
		break;

	case ENTITY_EVENT_START_GAME:
		MonoEventFunction("OnStartGame", NULL);
		break;

	case ENTITY_EVENT_ENTER_SCRIPT_STATE:
		MonoEventFunction("OnEnterScriptState", NULL);
		break;

	case ENTITY_EVENT_LEAVE_SCRIPT_STATE:
		MonoEventFunction("OnLeaveScriptState", NULL);
		break;

	case ENTITY_EVENT_PRE_SERIALIZE:
		MonoEventFunction("OnPreSerialize", NULL);
		break;

	case ENTITY_EVENT_POST_SERIALIZE:
		MonoEventFunction("OnPostSerialize", NULL);
		break;

	case ENTITY_EVENT_INVISIBLE:
		MonoEventFunction("OnInvisible", NULL);
		break;

	case ENTITY_EVENT_VISIBLE:
		MonoEventFunction("OnVisible", NULL);
		break;

	case ENTITY_EVENT_MATERIAL:
		void* args18[1];
		args18[0] = &evt.nParam[0];
		MonoEventFunction("OnMaterial", args18);
		break;

	case ENTITY_EVENT_MATERIAL_LAYER:
		MonoEventFunction("OnMaterialLayer", NULL);
		break;

	case ENTITY_EVENT_ONHIT:
		MonoEventFunction("OnHit", NULL);
		break;

	case ENTITY_EVENT_PICKUP:
		void* args19[3];
		args19[0] = &evt.nParam[0];
		args19[1] = &evt.nParam[1];
		args19[2] = &evt.fParam[0];
		MonoEventFunction("OnPickup", args19);
		break;

	case ENTITY_EVENT_ANIM_EVENT:
		void* args20[1];
		args20[0] = &evt.nParam[0];
		MonoEventFunction("OnAnimEvent", args20);
		break;

	case ENTITY_EVENT_SCRIPT_REQUEST_COLLIDERMODE:
		void* args21[1];
		args21[0] = &evt.nParam[0];
		MonoEventFunction("OnScriptRequestColliderMode", args21);
		break;

	case ENTITY_EVENT_INIT:
		// TODO?
		break;

	}
}
void CMonoEntity::SetChannelId(uint16 id)
{

}
void CMonoEntity::SetAuthority(bool auth)
{

}
void CMonoEntity::GetMemoryStatistics(ICrySizer * s)
{

}

CMonoEntity* CMonoEntity::GetById(EntityId id)
{
	std::vector<CMonoEntity*>::iterator it;
	for (it = CMonoEntity::m_monoEntities.begin(); it != m_monoEntities.end(); ++it)
	{
		if ((*it)->GetEntityId() == id)
			return (*it);
	}
	return NULL;
}