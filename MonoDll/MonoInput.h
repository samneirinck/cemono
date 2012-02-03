/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// IActionListener mono extension
//////////////////////////////////////////////////////////////////////////
// 02/02/2012 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/

#include <IMonoInput.h>
#include <IMonoAutoRegScriptBind.h>

#include <IActionMapManager.h>

#include "MonoCommon.h"

struct IMonoClass;

class CMonoInput 
	: public IMonoInput
	, public IActionListener
	, public IMonoScriptBind
{
public:
	CMonoInput();
	~CMonoInput();

	// IMonoScriptBind
	virtual void Release() override { delete this; }

	virtual const char *GetClassName() override { return "InputSystem"; }
	// ~IMonoScriptBind

	// IActionListener
	virtual void OnAction( const ActionId& action, int activationMode, float value );
	// ~IActionListener

private:
	MonoMethod(void, RegisterAction, mono::string);

	bool OnActionTriggered(EntityId entityId, const ActionId& actionId, int activationMode, float value);

	IMonoClass *m_pClass;
	
	static TActionHandler<CMonoInput>	s_actionHandler;
};
