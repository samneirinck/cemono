/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2008.
-------------------------------------------------------------------------

Description:  Item/weapon accessories

-------------------------------------------------------------------------
History:
- 28:1:2008   Created by Benito G.R. 

*************************************************************************/

#ifndef _ACCESSORY_H_
#define _ACCESSORY_H_

#include "Item.h"

class CAccessory: public CItem
{
public:

	CAccessory() {};
	virtual			~CAccessory() {};
	
	void PickUp(EntityId pickerId, bool sound, bool select/* =true */, bool keepHistory/* =true */, const char *setup = NULL);

	virtual bool IsAccessory() { return true; };

};

#endif
