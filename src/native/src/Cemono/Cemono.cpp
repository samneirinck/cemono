#include "stdafx.h"
#include "Cemono.h"
#include <CryExtension/Impl/ClassWeaver.h>

CRYREGISTER_CLASS(CCemono)

CCemono::CCemono()
{
}

CCemono::~CCemono()
{
}

bool CCemono::Init()
{
	CryLog("CCemono init");
	return true;
}

void CCemono::Shutdown()
{

}
