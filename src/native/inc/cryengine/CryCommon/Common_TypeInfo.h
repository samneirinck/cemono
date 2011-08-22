#include "TypeInfo_impl.h"

#include "CryHeaders_info.h"
#include "Cry_Vector2_info.h"
#include "Cry_Vector3_info.h"
#include "Cry_Quat_info.h"
#include "Cry_Matrix_info.h"
#include "Cry_Color_info.h"
#include "TimeValue_info.h"
#include "CryHalf_info.h"

#ifndef _LIB
	// Manually instantiate templates as needed here.
	#ifndef XENON_INTRINSICS
		template struct Vec3_tpl<float>;
		template struct Vec4_tpl<float>;
	#endif
	template struct Ang3_tpl<float>;
	template struct Plane_tpl<float>;
	template struct Matrix33_tpl<float>;
	template struct Color_tpl<float>;
#endif
