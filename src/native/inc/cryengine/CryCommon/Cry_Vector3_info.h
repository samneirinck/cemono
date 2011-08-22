#ifndef CRY_VECTOR3_INFO_H
#define CRY_VECTOR3_INFO_H

#include "Cry_Vector3.h"

STRUCT_INFO_T_BEGIN(Vec3_tpl, typename, F)
	VAR_INFO(x)
	VAR_INFO(y)
	VAR_INFO(z)
STRUCT_INFO_T_END(Vec3_tpl, typename, F)

STRUCT_INFO_T_BEGIN(Vec4_tpl, typename, F)
	VAR_INFO(x)
	VAR_INFO(y)
	VAR_INFO(z)
	VAR_INFO(w)
STRUCT_INFO_T_END(Vec4_tpl, typename, F)

STRUCT_INFO_T_BEGIN(Ang3_tpl, typename, F)
	VAR_INFO(x)
	VAR_INFO(y)
	VAR_INFO(z)
STRUCT_INFO_T_END(Ang3_tpl, typename, F)

STRUCT_INFO_T_BEGIN(Plane_tpl, typename, F)
	VAR_INFO(n)
	VAR_INFO(d)
STRUCT_INFO_T_END(Plane_tpl, typename, F)

#endif