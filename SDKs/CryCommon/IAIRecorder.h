#include DEVIRTUALIZE_HEADER_FIX(IAIRecorder.h)

#ifndef _IAIRECORDER_H_
#define _IAIRECORDER_H_

#pragma once

#ifdef LINUX
#include "platform.h"
#endif


// AI Recorder context for a concrete AI Object
struct SAIRecorderObjectContext
{
	string sName;
	bool bEnableDrawing;
	bool bSetView;
	ColorB color;

	SAIRecorderObjectContext() : bEnableDrawing(false), bSetView(false), color(255, 255, 255, 255) {}
};
typedef std::map<string, SAIRecorderObjectContext> TAIRecorderObjectsMap;


// AI Recorder context information from the Editor tool
struct SAIRecorderContext
{
	float fStartPos;
	float fEndPos;
	float fCursorPos;

	// List of objects to debug
	TAIRecorderObjectsMap DebugObjects;

	SAIRecorderContext() : fStartPos(0.f), fEndPos(0.f), fCursorPos(0.f) {}
};


// Recorder mode options
enum EAIRecorderMode
{
	eAIRM_Off = 0,

	eAIRM_Memory,
	eAIRM_Disk,
};


UNIQUE_IFACE struct IAIRecorderListener
{
	virtual ~IAIRecorderListener() {}

	virtual void OnAIRecordingStart(EAIRecorderMode eAIRecorderMode, const char* szFileName) {}
	virtual void OnAIRecordingStop(const char* szFileName) {}
	virtual void OnAIRecordingLoaded(const char* szFileName) {}
	virtual void OnAIRecordingSaved(const char* szFileName) {}
};


UNIQUE_IFACE struct IAIRecorder
{
	virtual ~IAIRecorder() {}

	virtual bool IsRunning() const = 0;

	virtual bool Load(const char* szFileName = NULL) = 0;
	virtual bool Save(const char* szFileName = NULL) const = 0;

	virtual bool AddListener(IAIRecorderListener* pAIRecorderListener) = 0;
	virtual bool RemoveListener(IAIRecorderListener* pAIRecorderListener) = 0;
};


enum EAIDebugEvent
{
	eAIDE_First,
	eAIDE_None,
	eAIDE_Reset,
	eAIDE_SignalReceived,
	eAIDE_AuxSignalReceived,
	eAIDE_SignalExecuting,
	eAIDE_GoalPipeSelected,
	eAIDE_GoalPipeInserted,
	eAIDE_GoalPipeReset,
	eAIDE_BehaviorSelected,
	eAIDE_BehaviorDestructor,
	eAIDE_BehaviorConstructor,
	eAIDE_AttentionTarget,
	eAIDE_AttentionTargetPos,
	eAIDE_StimulusRegistered,
	eAIDE_HandlerEvent,
	eAIDE_ActionStart,
	eAIDE_ActionSuspend,
	eAIDE_ActionResume,
	eAIDE_ActionEnd,
	eAIDE_Event,
	eAIDE_RefPointPos,
	eAIDE_AgentPos,
	eAIDE_AgentDir,
	eAIDE_LuaComment,
	eAIDE_Health,
	eAIDE_HitDamage,
	eAIDE_Death,
	eAIDE_SignalExecutedWarning,
	eAIDE_PressureGraph,
	eAIDE_Bookmark,
	eAIDE_Last,
};


struct IAIDebugStream
{
	virtual ~IAIDebugStream() {}

	virtual void	Seek(float fWhereTo) = 0;
	virtual int		GetCurrentIndex() = 0;	
	virtual int		GetSize() = 0;
	virtual void*	GetCurrent(float& fStartingFrom) = 0;
	virtual bool	GetCurrentString(string& sOut, float& fStartingFrom) = 0;
	virtual void*	GetNext(float& fStartingFrom) = 0;
	virtual float	GetStartTime() = 0;
	virtual float	GetEndTime() = 0;
	virtual bool	IsEmpty() = 0;

	virtual char const* GetName() const = 0;
};


struct SAIDebugEventData
{
	const char* szString;
	Vec3 pos;
	float	fValue;
	void* pExtraData;

	SAIDebugEventData(const char* szString) : szString(szString), pos(ZERO), fValue(0.f), pExtraData(NULL) {}
	SAIDebugEventData(const Vec3& pos) : szString(NULL), pos(pos), fValue(0.f), pExtraData(NULL) {}
	SAIDebugEventData(float fValue) : szString(NULL), pos(ZERO), fValue(fValue), pExtraData(NULL) {}
};


#endif	// #ifndef _IAIRECORDER_H_
