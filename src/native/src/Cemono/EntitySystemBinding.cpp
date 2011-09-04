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
	string name = CCemono::ToString(monoName);
	string editorHelper=  CCemono::ToString(monoEditorHelper);
	string editorIcon = CCemono::ToString(monoEditorIcon);
	string fullyQualifiedName = CCemono::ToString(monoFullyQualifiedName);
	string pathToAssembly = CCemono::ToString(monoPathToAssembly);
	string category = CCemono::ToString(monoCategory);

	int numProperties = mono_array_length(monoProperties);
	MonoClass* pClass = NULL;
	std::vector<IEntityPropertyHandler::SPropertyInfo> properties;

	for	(int i = 0; i < numProperties; ++i)
	{
		MonoObject* obj = mono_array_get(monoProperties, MonoObject*, i);

		if (!pClass)
		{
			pClass = mono_object_get_class(obj);

		}
			IEntityPropertyHandler::SPropertyInfo propertyInfo;

			// Horrible, needs a refactor
			propertyInfo.name = CCemono::ToString( (MonoString*)mono_property_get_value(mono_class_get_property_from_name(pClass, "Name"), obj, NULL,NULL));
			propertyInfo.description = CCemono::ToString( (MonoString*)mono_property_get_value(mono_class_get_property_from_name(pClass, "Description"), obj, NULL,NULL));
			propertyInfo.editType = CCemono::ToString( (MonoString*)mono_property_get_value(mono_class_get_property_from_name(pClass, "EditorType"), obj, NULL,NULL));
			propertyInfo.flags = *(int*)mono_object_unbox(mono_property_get_value(mono_class_get_property_from_name(pClass, "Flags"), obj, NULL, NULL));
			propertyInfo.type = (IEntityPropertyHandler::EPropertyType)*(int*)mono_object_unbox(mono_property_get_value(mono_class_get_property_from_name(pClass, "Type"), obj, NULL, NULL));
			propertyInfo.limits.min = *(float*)mono_object_unbox(mono_property_get_value(mono_class_get_property_from_name(pClass, "MinValue"), obj, NULL, NULL));
			propertyInfo.limits.max = *(float*)mono_object_unbox(mono_property_get_value(mono_class_get_property_from_name(pClass, "MaxValue"), obj, NULL, NULL));

			properties.push_back(propertyInfo);
	}

	IEntityClassRegistry::SEntityClassDesc entityClassDesc;
	entityClassDesc.flags = flags;
	entityClassDesc.sName = name;
	entityClassDesc.sEditorHelper = editorHelper;
	entityClassDesc.sEditorIcon = editorIcon;
	//gEnv->pEntitySystem->GetClassRegistry()->RegisterStdClass(entityClassDesc);
	
	CCemonoEntityClass* entityClass = new CCemonoEntityClass(entityClassDesc, category, properties);



	bool result = gEnv->pEntitySystem->GetClassRegistry()->RegisterClass(entityClass);


}