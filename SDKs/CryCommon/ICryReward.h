#if !defined(__ICRYREWARD_H__)
#define __ICRYREWARD_H__

enum ECryRewardError
{
	eCRE_Queued = 0,		// Reward successfully queued
	eCRE_Busy,					// Reward queue full - try again later
	eCRE_Failed					// Reward process failed
};


// CryRewardCallback
// taskID		-	Task ID allocated when the function was executed
// error		- Error code - eCLE_Success if the function succeeded or an error that occurred while processing the function
// pArg			- Pointer to application-specified data
typedef void (*CryRewardCallback)(CryLobbyTaskID taskID, uint32 playerID, uint32 awardID, ECryLobbyError error, void* pArg);



struct ICryReward
{
	virtual ~ICryReward(){}
	// Award
	// Awards an achievement/trophy/reward to the specified player
	// playerID	- player ID
	// awardID	- award ID (probably implemented as an enumerated type)
	// return		- informs the caller that the award was added to the pending queue or not
	virtual ECryRewardError		Award(uint32 playerID, uint32 awardID, CryLobbyTaskID* pTaskID, CryRewardCallback cb, void* pCbArg) = 0;
};

#endif // End [!defined(__ICRYREWARD_H__)]
// [EOF]