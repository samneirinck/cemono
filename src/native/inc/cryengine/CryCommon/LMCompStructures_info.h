#include "LMCompStructures.h"

STRUCT_INFO_BEGIN(LMFalseLightFileHeader)
	STRUCT_VAR_INFO(iVersion, TYPE_INFO(uint8))
	STRUCT_VAR_INFO(iNumObjects, TYPE_INFO(uint32))
STRUCT_INFO_END(LMFalseLightFileHeader)

STRUCT_INFO_BEGIN(sFalseLightSerializationStructure)
	STRUCT_VAR_INFO(m_nObjectID, TYPE_INFO(int32))
	STRUCT_VAR_INFO(m_nLightNumber, TYPE_INFO(int32))
	STRUCT_VAR_INFO(m_Lights, TYPE_ARRAY(32, TYPE_INFO(EntityId)))
STRUCT_INFO_END(sFalseLightSerializationStructure)

STRUCT_INFO_BEGIN(TexCoord2Comp)
	STRUCT_VAR_INFO(s, TYPE_INFO(float))
	STRUCT_VAR_INFO(t, TYPE_INFO(float))
STRUCT_INFO_END(TexCoord2Comp)

