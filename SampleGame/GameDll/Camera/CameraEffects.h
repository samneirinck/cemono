/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2008.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Camera effects

-------------------------------------------------------------------------
History:
- 12:2008 : Created By Jan Müller

*************************************************************************/

#ifndef CAMERA_EFFECTS_H
#define CAMERA_EFFECTS_H

class CCameraEffects
{
public:
	//controls tree/vegetation transparency
	static void UpdateTreeTransparency(const Vec3 &camPos, const Vec3 &targetPos);

	//sets the depth of field effect
	static void UpdateDOF();
};

#endif
