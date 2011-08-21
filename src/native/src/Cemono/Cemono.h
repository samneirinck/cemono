#pragma once

#include "Headers/ICemono.h"
#include <CryExtension/Impl/ClassWeaver.h>

class CCemono : public ICemono
{
	CRYINTERFACE_BEGIN()
	CRYINTERFACE_ADD(ICemono)
	CRYINTERFACE_END()
	
	CRYGENERATE_SINGLETONCLASS(CCemono, "Cemono", 0xc37b8ad5d62f47de, 0xa8debe525ff0fc8a)

public:
	virtual bool Init();
	virtual void Shutdown();
};