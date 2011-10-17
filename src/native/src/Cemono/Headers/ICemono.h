#pragma once

#include <CryExtension/ICryUnknown.h>
#include <IGameRulesSystem.h>
#include "ICemonoClassBinding.h"

struct ICemono : public ICryUnknown
{
	CRYINTERFACE_DECLARE(ICemono, 0x86169744ce38420f, 0x9768a98386be991f)


	virtual bool Init() = 0 ;
	virtual void Shutdown() = 0;
	virtual void AddClassBinding(std::shared_ptr<ICemonoClassBinding> pBinding) = 0;
	virtual void PostInit() = 0;

	typedef void *(*TEntryFunction)(ISystem* pSystem);

};

typedef boost::shared_ptr<ICemono> ICemonoPtr;
