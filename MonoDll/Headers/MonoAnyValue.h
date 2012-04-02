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
	MONOTYPE_WSTRING,

	MONOTYPE_LAST
};

/// <summary>
/// Simple class used to easily convert common C++ types to their C# equivalents.
/// Serialization support is built-in, although untested at the moment. Will be functional in time for CryMono 0.3.
/// </summary>
struct MonoAnyValue
{
	MonoAnyValue() : type(MONOTYPE_NULL) { };
	MonoAnyValue(bool value) : type(MONOTYPE_BOOL) { b = value; }
	MonoAnyValue(int value) : type(MONOTYPE_INT) { i = value; }
	MonoAnyValue(unsigned int value) : type(MONOTYPE_UINT) { u = value; }
	MonoAnyValue(short value) : type(MONOTYPE_SHORT) { i = value; }
	MonoAnyValue(unsigned short value) : type(MONOTYPE_USHORT) { u = value; }
	MonoAnyValue(float value) : type(MONOTYPE_FLOAT) { f = value; }
	MonoAnyValue(const char *value) : type(MONOTYPE_STRING) { str = value; }
	MonoAnyValue(const wchar_t *value) : type(MONOTYPE_WSTRING) { wstr = value; }
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
		case MONOTYPE_USHORT:
			ser.Value("monoValueUIntNum", u);
			break;
		case MONOTYPE_INT:
		case MONOTYPE_SHORT:
			ser.Value("monoValueIntNum", i);
			break;
		case MONOTYPE_FLOAT:
			ser.Value("monoValueFloatNum", f);
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
		case MONOTYPE_WSTRING:
			break;
		}

		ser.EnumValue("monoValueType", type, MONOTYPE_NULL, MONOTYPE_LAST);
		ser.EndGroup();
	};

	MonoAnyType type;
	union
	{
		bool			b;
		float			f;
		int				i;
		unsigned int	u;
		const char*		str;
		const wchar_t*	wstr;
		struct { float x,y,z; } vec3;
	};
};

#endif //__MONO_ANY_VALUE__