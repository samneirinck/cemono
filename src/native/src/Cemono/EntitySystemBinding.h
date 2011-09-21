#pragma once

#include "BaseCemonoClassBinding.h"

class CEntitySystemBinding : public BaseCemonoClassBinding
{
public:
	CEntitySystemBinding();
	virtual ~CEntitySystemBinding();

protected:
	virtual const char* GetClassName() override { return "EntitySystem"; }

	static void _RegisterEntityClass(int flags, MonoString* name, MonoString* editorHelper, MonoString* editorIcon, MonoString* category, MonoString* fullyQualifiedName, MonoString* pathToAssembly, MonoArray* properties);

};

