/////////////////////////////////////////////////////////////////////////*
//Ink Studios Source File.
//Copyright (C), Ink Studios, 2011.
//////////////////////////////////////////////////////////////////////////
// Used to easily get any type of value from C++ to C#,
// also used to serialize C# objects for RMI's.
//////////////////////////////////////////////////////////////////////////
// 31/12/2011 : Created by Filip 'i59' Lundgren
////////////////////////////////////////////////////////////////////////*/
#ifndef __MONO_ANY_VALUE__
#define __MONO_ANY_VALUE__

/// <summary>
/// Used within MonoAnyValue and IMonoObject to easily get the object type contained within.
/// </summary>
enum MonoAnyType
{
	MONOTYPE_NULL = -1,

	MONOTYPE_BOOL,

	MONOTYPE_INT,
	MONOTYPE_UINT,
	MONOTYPE_SHORT,
	MONOTYPE_USHORT,
	MONOTYPE_FLOAT,
	MONOTYPE_VEC3,

	MONOTYPE_STRING,

	MONOTYPE_LAST
};

/// <summary>
/// Simple class used to easily convert common C++ types to their C# equivalents.
/// Serialization support is built-in, although untested at the moment. Will be functional in time for CryMono 0.3.
/// </summary>
struct MonoAnyValue
{
	MonoAnyValue(bool value) : type(MONOTYPE_BOOL) { b = value; }
	MonoAnyValue(int value) : type(MONOTYPE_INT) { number = (float)value; }
	MonoAnyValue(unsigned int value) : type(MONOTYPE_UINT) { number = (float)value; }
	MonoAnyValue(short value) : type(MONOTYPE_SHORT) { number = (float)value; }
	MonoAnyValue(unsigned short value) : type(MONOTYPE_USHORT) { number = (float)value; }
	MonoAnyValue(float value) : type(MONOTYPE_FLOAT) { number = value; }
	MonoAnyValue(const char *value) : type(MONOTYPE_STRING) { str = value; }
	MonoAnyValue(string value) : type(MONOTYPE_STRING) { str = value.c_str(); }
	MonoAnyValue(Vec3 value) : type(MONOTYPE_VEC3) { vec3.x = value.x; vec3.y = value.y; vec3.z = value.z; }
	MonoAnyValue(Ang3 value) : type(MONOTYPE_VEC3) { vec3.x = value.x; vec3.y = value.y; vec3.z = value.z; }

	void Serialize(TSerialize ser)
	{
		ser.BeginGroup("MonoAnyValue");
		switch(type)
		{
		case MONOTYPE_BOOL:
			ser.Value("monoValueBool", b);
			break;

		case MONOTYPE_UINT:
		case MONOTYPE_INT:
		case MONOTYPE_FLOAT:
		case MONOTYPE_SHORT:
		case MONOTYPE_USHORT:
			ser.Value("monoValueNum", number);
			break;
		case MONOTYPE_VEC3:
			{
				ser.Value("monoValueVec3x", vec3.x);
				ser.Value("monoValueVec3y", vec3.y);
				ser.Value("monoValueVec3z", vec3.z);
			}
			break;
		case MONOTYPE_STRING:
			ser.ValueChar("monoValueString", const_cast<char *>(str), strlen(str));
			break;
		}

		ser.EnumValue("monoValueType", type, MONOTYPE_NULL, MONOTYPE_LAST);
		ser.EndGroup();
	};

	MonoAnyType type;
	union
	{
		bool b;
		float number;
		const char *str;
		struct { float x,y,z; } vec3;
	};
};

#endif //__MONO_ANY_VALUE__