#pragma once

#include <IGameFramework.h>
#include <IEntitySystem.h>

class CCemonoGameRulesListener : public IEntityEventListener
{
public:
	CCemonoGameRulesListener();
	virtual ~CCemonoGameRulesListener();

	// IEntityEventListener
	virtual void OnEntityEvent( IEntity *pEntity,SEntityEvent &event ) override;
	// -IEntityEventListsener


protected:

};