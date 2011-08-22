#ifndef __ISelectionTreeManager_h__
#define __ISelectionTreeManager_h__

#pragma  once


struct ISelectionTreeManager
{
	virtual ~ISelectionTreeManager(){}
	virtual uint32 GetSelectionTreeCount() const = 0;
	virtual uint32 GetSelectionTreeCountOfType(const char* typeName) const = 0;

	virtual const char* GetSelectionTreeName(uint32 index) const = 0;
	virtual const char* GetSelectionTreeNameOfType(const char* typeName, uint32 index) const = 0;
};

#endif