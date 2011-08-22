#include "Cry_Quat.h"

STRUCT_INFO_T_BEGIN(Quat_tpl, typename, F)
	STRUCT_VAR_INFO(v, TYPE_INFO(Vec3_tpl<F>))
	STRUCT_VAR_INFO(w, TYPE_INFO(F))
STRUCT_INFO_T_END(Quat_tpl, typename, F)

STRUCT_INFO_T_BEGIN(QuatT_tpl, typename, F)
	STRUCT_VAR_INFO(q, TYPE_INFO(Quat_tpl<F>))
	STRUCT_VAR_INFO(t, TYPE_INFO(Vec3_tpl<F>))
STRUCT_INFO_T_END(QuatT_tpl, typename, F)

STRUCT_INFO_T_BEGIN(QuatTS_tpl, typename, F)
	STRUCT_VAR_INFO(q, TYPE_INFO(Quat_tpl<F>))
	STRUCT_VAR_INFO(t, TYPE_INFO(Vec3_tpl<F>))
	STRUCT_VAR_INFO(s, TYPE_INFO(F))
STRUCT_INFO_T_END(QuatTS_tpl, typename, F)









