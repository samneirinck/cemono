#include "stdafx.h"
#include <CryUnitTest.h>

CRY_UNIT_TEST_SUITE(Cemono)
{
	CRY_UNIT_TEST_NAME(Cemono, "Initialization")
	{
		CRY_UNIT_TEST_ASSERT(true);

		CRY_UNIT_TEST_ASSERT(gEnv);
	}
}