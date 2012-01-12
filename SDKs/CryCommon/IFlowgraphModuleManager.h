/********************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2011.
-------------------------------------------------------------------------
File name:   IFlowgraphModuleManager.h
$Id$
Description: IFlowgraphModuleManager interface

-------------------------------------------------------------------------
History:
- 03/04/11   : Sascha Hoba - Kevin Kirst 

*********************************************************************/

#ifndef FLOWGRAPHMODULEMANAGERINTERFACE_H
#define FLOWGRAPHMODULEMANAGERINTERFACE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct IModuleIterator
{
	virtual ~IModuleIterator(){}
	virtual size_t  Count() = 0;
	virtual string Next() = 0;
	virtual void    AddRef() = 0;
	virtual void    Release() = 0;
};
typedef _smart_ptr<IModuleIterator> IModuleIteratorPtr;

struct IFlowgraphModuleManager
{
	virtual bool DeleteModuleXML(const char* moduleName) = 0;
	virtual bool RenameModuleXML(const char* moduleName, const char* newName) = 0;
	virtual bool AddModulePathInfo(const char* path) = 0;
	virtual IModuleIteratorPtr CreateModuleIterator() = 0;
	virtual const char* GetModuleFolderName() const = 0;
	virtual const char* GetStartNodeName() const = 0;
	virtual const char* GetReturnNodeName() const = 0;
	virtual const char* GetCallerNodeName() const = 0;
	virtual int GetModuleMaxParams() const = 0;
	virtual void ScanForModules() = 0;
	virtual const char* GetModulePath(const char* name) = 0;
	virtual bool ReloadModule(const char* moduleName) = 0;
};

#endif //FLOWGRAPHMODULEMANAGERINTERFACE_H