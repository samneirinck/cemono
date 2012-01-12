/* 
	interface definition for spu job manager
*/

#ifndef __IJOBMAN_SPU_H
#define __IJOBMAN_SPU_H
#pragma once

struct ILog;

#ifndef _ALIGN
	#if defined __CRYCG__
		#define _ALIGN(num) __attribute__ ((aligned(num))) __attribute__ ((crycg_attr (aligned, num)))
	#else
		#define _ALIGN(num) __attribute__ ((aligned(num)))
	#endif
#endif

//#define PROVIDE_DEPENDENTJOB_API

//state of spu
#define SPUWaitState	  0xFCFCFCFC
#define SPUPollState	  0xBEBEBEBE
#define SPUThreadState	0xFEFEFEFE		//threading status written by each SPU
#define SPURunState			0							//running status written by each SPU

#if !defined(assert)
#define assert(condition) ((void)0)
#endif

namespace NPPU
{
	//page mode
	enum EPageMode
	{
		ePM_Single = 0,		//single page mode, job occupies as much as it takes
		ePM_Dual	 = 1,		//2 pages
		ePM_Quad	 = 2,		//4 pages
	};
}



































































































































































































































































































































































/*
//provide an implementation which can be called on any non ps3 platform evaluating to nothing
//avoids having nasty preprocessor defs

struct IJobManSPU
{
	const unsigned int GetSPUsAllowed() const{return 0;}
	void SetSPUsAllowed(const unsigned int)const{}
	const unsigned int GetDriverSize() const{return 0;}
	const bool InitSPUs(const char*)const{return true;}
	const bool SPUJobsActive() const{return false;}
	void ShutDown()const{}
	void TestSPUs() const{}
	void UpdateSPUMemMan()const{}
	void SetLog(ILog*)const{}
};

inline IJobManSPU *GetIJobManSPU()
{
	static IJobManSPU sJobManDummy;
	return &sJobManDummy;
}
*/

#endif //__IJOBMAN_SPU_H
	
