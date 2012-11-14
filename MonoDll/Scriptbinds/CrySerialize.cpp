#include "stdafx.h"
#include "CrySerialize.h"

CScriptbind_CrySerialize::CScriptbind_CrySerialize()
{
	REGISTER_METHOD(BeginGroup);
	REGISTER_METHOD(EndGroup);

	REGISTER_METHOD(ValueString);
	REGISTER_METHOD(ValueInt);
	REGISTER_METHOD(ValueBool);
	REGISTER_METHOD(ValueEntityId);
	REGISTER_METHOD(ValueFloat);
	REGISTER_METHOD(ValueVec3);
	REGISTER_METHOD(ValueQuat);

	REGISTER_METHOD(EnumValue);

	REGISTER_METHOD(IsReading);
}

CScriptbind_CrySerialize::~CScriptbind_CrySerialize()
{
}

void CScriptbind_CrySerialize::BeginGroup(ISerialize *ser, mono::string name)
{
	ser->BeginGroup(ToCryString(name));
}

void CScriptbind_CrySerialize::EndGroup(ISerialize *ser)
{
	ser->EndGroup();
}

void CScriptbind_CrySerialize::ValueString(ISerialize *ser, mono::string name, mono::string &str, int policy)
{
	TSerialize serialize = TSerialize(ser);
	if(!ser->IsReading())
		serialize.Value(ToCryString(name), ToCryString(str), policy);
	else
	{
		string cryStr;
		serialize.Value(ToCryString(name), cryStr, policy);
		str = ToMonoString(cryStr.c_str());
	}
}

void CScriptbind_CrySerialize::ValueInt(ISerialize *ser, mono::string name, int &obj, int policy)
{
	TSerialize serialize = TSerialize(ser);
	serialize.Value(ToCryString(name), obj, policy);
}

void CScriptbind_CrySerialize::ValueBool(ISerialize *ser, mono::string name, bool &obj)
{
	TSerialize serialize = TSerialize(ser);
	serialize.Value(ToCryString(name), obj, 'bool');
}

void CScriptbind_CrySerialize::ValueEntityId(ISerialize *ser, mono::string name, EntityId &obj)
{
	TSerialize serialize = TSerialize(ser);
	serialize.Value(ToCryString(name), obj, 'eid');
}

void CScriptbind_CrySerialize::ValueFloat(ISerialize *ser, mono::string name, float &obj, int policy)
{
	TSerialize serialize = TSerialize(ser);
	serialize.Value(ToCryString(name), obj, policy);
}

void CScriptbind_CrySerialize::ValueVec3(ISerialize *ser, mono::string name, Vec3 &obj, int policy)
{
	TSerialize serialize = TSerialize(ser);
	serialize.Value(ToCryString(name), obj, policy);
}

void CScriptbind_CrySerialize::ValueQuat(ISerialize *ser, mono::string name, Quat &obj, int policy)
{
	TSerialize serialize = TSerialize(ser);
	serialize.Value(ToCryString(name), obj, policy);
}

void CScriptbind_CrySerialize::EnumValue(ISerialize *ser, mono::string name, int &obj, int first, int last)
{
	TSerialize serialize = TSerialize(ser);
	serialize.EnumValue(ToCryString(name), obj, first, last);
}

bool CScriptbind_CrySerialize::IsReading(ISerialize *ser)
{
	return ser->IsReading();
}