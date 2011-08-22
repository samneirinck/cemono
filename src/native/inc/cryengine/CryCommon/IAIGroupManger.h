#ifndef __IAIGroupManger_h__
#define __IAIGroupManger_h__

#pragma once

typedef int GroupID;

struct IAIGroupManager
{
	virtual ~IAIGroupManager(){}
	virtual void SetGroupVariable(GroupID groupID, const char* variableName, bool value) = 0;
	virtual bool GetGroupVariable(GroupID groupID, const char* variableName) const = 0;
};

#endif // __IAIGroupManger_h__