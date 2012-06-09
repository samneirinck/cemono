#include "StdAfx.h"
#include "MonoEntityPropertyHandler.h"

#include "MonoEntity.h"

#include <MonoCommon.h>
#include <IMonoScriptSystem.h>
#include <IMonoObject.h>

#include <IEntitySystem.h>

CEntityPropertyHandler::CEntityPropertyHandler(std::vector<IEntityPropertyHandler::SPropertyInfo> properties)
	: m_properties(properties)
{
}

int CEntityPropertyHandler::GetPropertyCount() const
{
	return m_properties.size();
}

bool CEntityPropertyHandler::GetPropertyInfo(int index, SPropertyInfo& info ) const
{
	info = m_properties.at(index);
	return true;
}

SQueuedProperty *CEntityPropertyHandler::GetQueuedProperties(EntityId id, int &numProperties)
{
	for each(auto pair in m_queuedProperties)
	{
		if(pair.first == id)
		{
			numProperties = pair.second.size();
			SQueuedProperty *pProperties = new SQueuedProperty[numProperties];

			int i = 0;
			for(auto it = pair.second.begin(); it != pair.second.end(); ++it)
			{
				pProperties[i] = *it;

				i++;
			}

			return pProperties;
		}
	}

	return NULL;
}

void CEntityPropertyHandler::SetProperty(IEntity *pIEntity, int index, const char *value)
{
	EntityId id = pIEntity->GetId();

	CEntity *pEntity = NULL;
	if(IGameObject *pGameObject = gEnv->pGameFramework->GetGameObject(id))
		pEntity = static_cast<CEntity *>(pGameObject->QueryExtension(pIEntity->GetClass()->GetName()));

	// Only true after game has started, limiting this to changes made in Editor.
	if(pEntity && pEntity->IsInitialized())
		pEntity->SetPropertyValue(m_properties.at(index), value);
	else
	{
		bool exists = false;
		for(auto it = m_queuedProperties.begin(); it != m_queuedProperties.end(); ++it)
		{
			if((*it).first == id)
			{
				(*it).second.push_back(SQueuedProperty(m_properties.at(index), value));

				exists = true;
				break;
			}
		}

		if(!exists)
		{
			DynArray<SQueuedProperty> queuedPropertiesForEntity;
			queuedPropertiesForEntity.push_back(SQueuedProperty(m_properties.at(index), value));

			m_queuedProperties.insert(TQueuedPropertyMap::value_type(id, queuedPropertiesForEntity));
		}
	}
}

const char *CEntityPropertyHandler::GetProperty(IEntity *pEntity, int index) const
{
	//if(IMonoClass *pScriptClass = gEnv->pMonoScriptSystem->GetEntityManager()->GetScript(pEntity->GetId()))
		//return CallMonoScript<const char *>(pScriptClass, "GetPropertyValue", m_properties.at(index).name);

	return "";
}