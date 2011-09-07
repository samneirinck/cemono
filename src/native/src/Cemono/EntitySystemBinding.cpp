#include "StdAfx.h"
#include "EntitySystemBinding.h"
#include "CemonoEntityClass.h"

#include <IEntitySystem.h>
#include <IEntityClass.h>
#include <IGame.h>
#include <IGameFramework.h>


CEntitySystemBinding::CEntitySystemBinding()
{
	REGISTER_METHOD(_RegisterEntityClass);
}


CEntitySystemBinding::~CEntitySystemBinding()
{
}

void CEntitySystemBinding::_RegisterEntityClass(int flags, MonoString* monoName, MonoString* monoEditorHelper, MonoString* monoEditorIcon, MonoString* monoCategory, MonoString* monoFullyQualifiedName, MonoString* monoPathToAssembly, MonoArray* monoProperties)
{
	string name = CCemonoString::ToString(monoName);
	string editorHelper=  CCemonoString::ToString(monoEditorHelper);
	string editorIcon = CCemonoString::ToString(monoEditorIcon);
	string fullyQualifiedName = CCemonoString::ToString(monoFullyQualifiedName);
	string pathToAssembly = CCemonoString::ToString(monoPathToAssembly);
	string category = CCemonoString::ToString(monoCategory);
	CCemonoArray propertiesArray(monoProperties);

	int numProperties = propertiesArray.GetLength();
	MonoClass* pClass = NULL;
	std::vector<IEntityPropertyHandler::SPropertyInfo> properties;

	for	(int i = 0; i < numProperties; ++i)
	{
		CCemonoObject obj = propertiesArray.GetItem(i);

		if (!pClass)
		{
			pClass = obj.GetClass();

		}
		IEntityPropertyHandler::SPropertyInfo propertyInfo;

		propertyInfo.name = obj.GetPropertyValue<string>("Name");
		propertyInfo.description = obj.GetPropertyValue<string>("Description");
		propertyInfo.editType = obj.GetPropertyValue<string>("EditorType");
		propertyInfo.type = (IEntityPropertyHandler::EPropertyType)obj.GetPropertyValueAndUnbox<int>("Type");
		propertyInfo.limits.min = obj.GetPropertyValueAndUnbox<float>("MinValue");
		propertyInfo.limits.max = obj.GetPropertyValueAndUnbox<float>("MaxValue");

		properties.push_back(propertyInfo);
	}

	IEntityClassRegistry::SEntityClassDesc entityClassDesc;
	entityClassDesc.flags = flags;
	entityClassDesc.sName = name;
	entityClassDesc.sEditorHelper = editorHelper;
	entityClassDesc.sEditorIcon = editorIcon;
	
	CCemonoEntityClass* entityClass = new CCemonoEntityClass(entityClassDesc, category, properties);

	bool result = gEnv->pEntitySystem->GetClassRegistry()->RegisterClass(entityClass);
}