#pragma once

#include "CemonoObject.h"
#include <mono/jit/jit.h>

class CCemonoArray
{
public:
	CCemonoArray(MonoArray* pArray);
	virtual ~CCemonoArray();

	int GetLength() const;
	CCemonoObject GetItem(int index);

protected:
	MonoArray* m_pArray;
};

