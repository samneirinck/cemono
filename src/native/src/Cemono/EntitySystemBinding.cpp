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
	const char* name = mono_string_to_utf8(monoName);
	const char* editorHelper=  mono_string_to_utf8(monoEditorHelper);
	const char* editorIcon = mono_string_to_utf8(monoEditorIcon);
	const char* fullyQualifiedName = mono_string_to_utf8(monoFullyQualifiedName);
	const char* pathToAssembly = mono_string_to_utf8(monoPathToAssembly);
	const char* category = mono_string_to_utf8(monoCategory);

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
			propertyInfo.name = mono_string_to_utf8( (MonoString*)mono_property_get_value(mono_class_get_property_from_name(pClass, "Name"), obj, NULL,NULL));
			propertyInfo.description = mono_string_to_utf8( (MonoString*)mono_property_get_value(mono_class_get_property_from_name(pClass, "Description"), obj, NULL,NULL));
			propertyInfo.editType = mono_string_to_utf8( (MonoString*)mono_property_get_value(mono_class_get_property_from_name(pClass, "EditorType"), obj, NULL,NULL));
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