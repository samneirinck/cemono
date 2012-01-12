////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2010.
// -------------------------------------------------------------------------
//  File name:   IScaleformGFx.h
//  Version:     v1.00
//  Created:     17/6/2010 by Paul Reindell.
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////
#include DEVIRTUALIZE_HEADER_FIX(IScaleformGFx.h)

#ifndef __IScaleformGFx_H__
#define __IScaleformGFx_H__

#include "IFlashPlayer.h"

struct IFlashLoadMovieHandler;

UNIQUE_IFACE struct IScaleformGFx
{
	virtual ~IScaleformGFx() {}

	virtual bool IsScaleformSupported() const = 0;
	virtual void Release() = 0;
	virtual IFlashPlayer* CreateFlashPlayerInstance() const = 0;
	virtual IFlashPlayerBootStrapper* CreateFlashPlayerBootStrapper() const = 0;
	virtual void RenderFlashInfo() = 0;
	virtual void GetFlashMemoryUsage(ICrySizer* pSizer) = 0;
	virtual void SetFlashLoadMovieHandler(IFlashLoadMovieHandler* pHandler) const = 0;
	virtual void GetFlashProfileResults(float& accumTime, bool /*reset*/) const = 0;
	virtual void GetFlashRenderStats(unsigned & numDPs, unsigned int& numTris) = 0;
	virtual void SetThreadIDs(uint32 mainThreadID, uint32 renderThreadID) = 0;
	virtual void AmpEnable(bool bEnable) = 0;
	virtual void AmpAdvanceFrame() = 0;
};

#endif // ifndef __IScaleformGFx_H_