////////////////////////////////////////////////////////////////////////////
//
//  CryEngine Source File.
//  Copyright (C), Crytek Studios, 2001-2010.
// -------------------------------------------------------------------------
//  File name:   IStatoscope.h
// -------------------------------------------------------------------------
//
////////////////////////////////////////////////////////////////////////////

#ifndef __IStatoscope_h__
#define __IStatoscope_h__
#pragma once

#include DEVIRTUALIZE_HEADER_FIX(IStatoscope.h)

#if ENABLE_STATOSCOPE

struct STexturePoolAllocation;

// Statoscope interface, access through gEnv->pStatoscope
UNIQUE_IFACE struct IStatoscope
{
	virtual ~IStatoscope(){}
private:
	//Having variables in an interface is generally bad design, but having this here avoids several virtual function calls from the renderer that may be repeated thousands of times a frame.
	bool m_bIsRunning;

public:
	IStatoscope(): m_bIsRunning(false) {}

	virtual void Tick() = 0;
	virtual void AddUserMarker(const char *name) = 0;	// a copy of name is taken, so it doesn't need to persist
	virtual void LogCallstack(const char *tag) = 0;		// likewise with tag
	virtual void LogCallstackFormat(const char* tagFormat, ...) = 0;
	virtual void SetCurrentProfilerRecords(const std::vector<CFrameProfiler*> *profilers) = 0;
	virtual void SetCurrentTexturePoolAllocations(std::vector<STexturePoolAllocation*> * allocations) = 0;
	virtual void DumpFrameRecords(bool bOutputFormat = false) = 0;
	virtual bool NeedsGPUTimers() = 0;
	inline bool IsRunning() {return m_bIsRunning;}
	inline void SetIsRunning( const bool bIsRunning ){m_bIsRunning = bIsRunning;}
	virtual void SetupFPSCaptureCVars() = 0;
	virtual bool RequestScreenShot() = 0;
};

#else // ENABLE_STATOSCOPE

struct IStatoscope
{
	void Tick() {}
	void AddUserMarker(const char *name) {}
	void LogCallstack(const char *tag) {}
	void LogCallstackFormat(const char* tagFormat, ...) {}
	void SetCurrentProfilerRecords(const std::vector<CFrameProfiler*> *profilers) {}
	void SetCurrentTexturePoolAllocations(std::vector<STexturePoolAllocation*> * allocations) {};
	void DumpFrameRecords(bool bOutputFormat = false) {}
	bool NeedsGPUTimers() { return false; }
	bool IsRunning() {return false;}
	void SetIsRunning( const bool bIsRunning ) {};
	void SetupFPSCaptureCVars() {}
	bool RequestScreenShot() {return false;}
};

#endif // ENABLE_STATOSCOPE

#endif  // __IStatoscope_h__
