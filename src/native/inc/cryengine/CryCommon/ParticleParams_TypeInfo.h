////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   ParticleParamsTypeInfo.h
// -------------------------------------------------------------------------
// Implements TypeInfo for ParticleParams. 
// Include only once per executable.
//
////////////////////////////////////////////////////////////////////////////

#ifdef ENABLE_TYPE_INFO_NAMES
	#if !ENABLE_TYPE_INFO_NAMES
		#error ENABLE_TYPE_INFO_NAMES previously defined to 0
	#endif
#else
	#define ENABLE_TYPE_INFO_NAMES	1
#endif

#include "TypeInfo_impl.h"
#include "IShader_info.h"
#include "I3DEngine_info.h"
#include "Cry_Vector3_info.h"
#include "Cry_Geo_info.h"
#include "ParticleParams_info.h"
#include "Name_TypeInfo.h"
#include "CryTypeInfo.h"

// Implementation of TCurveSpline<T> functions.

	// Helper class for serialization.
	template<class T>
	struct SplineElem
	{
		float fTime;
		T			Value;
		int		nFlags;
		STRUCT_INFO
	};

	// Manually define type info.
	STRUCT_INFO_T_BEGIN(SplineElem, class, T)
		VAR_INFO(fTime)
		VAR_INFO(Value)
		VAR_INFO(nFlags)
	STRUCT_INFO_T_END(SplineElem, class, T)


	template<class S>
	string TCurve<S>::ToString( FToString flags ) const
	{
		string str;
		for (int i = 0; i < this->num_keys(); i++)
		{
			if (i > 0)
				str += ";";
			SplineElem<S> elem = { this->key(i).time, this->key(i).value, this->key(i).flags };
			str += ::TypeInfo(&elem).ToString(&elem, flags);
		}
		return str;
	}

	template<class S>
	bool TCurve<S>::FromString( cstr str, FFromString flags )
	{
		CryStackStringT<char,256> strTemp;

		this->resize(0);
		while (*str)
		{
			// Extract element string.
			while (*str == ' ')
				str++;
			cstr strElem = str;
			if (cstr strEnd = strchr(str,';'))
			{
				strTemp.assign(str, strEnd);
				strElem = strTemp;
				str = strEnd+1;
			}
			else
				str = "";

			// Parse element.
			SplineElem<T> elem = { 0.f, T(0.f), 0 };
			if (!::TypeInfo(&elem).FromString(&elem, strElem))
				return false;

			// Fix any values somehow erroneously serialised.
			Limit(elem.fTime, 0.f, 1.f);
			if (max(elem.Value, T(0.f)) != elem.Value
			|| min(elem.Value, T(1.f)) != elem.Value)
				elem.Value = T(0.f);
			if (elem.nFlags & SPLINE_KEY_TANGENT_UNIFY_MASK)
				// Obsolete convention, now slope type settable per side.
				elem.nFlags |= (SPLINE_KEY_TANGENT_LINEAR << SPLINE_KEY_TANGENT_IN_SHIFT) 
										| (SPLINE_KEY_TANGENT_LINEAR << SPLINE_KEY_TANGENT_OUT_SHIFT);
			elem.nFlags &= (SPLINE_KEY_TANGENT_IN_MASK | SPLINE_KEY_TANGENT_OUT_MASK);

			int nKey = insert_key(elem.fTime, elem.Value);
			this->key(nKey).flags = elem.nFlags;
		};

		this->update();
		if (flags._Finalize)
			this->finalize();
		return true;
	}

	template<class S>
	void TCurve<S>::SerializeSpline( XmlNodeRef &node, bool bLoading )
	{
		if (bLoading)
			FromString( node->getAttr( "Keys" ) );
		else
			node->setAttr( "Keys", ToString(FToString().SkipDefault().TruncateSub()) );
	}

// Implementation of CSurfaceTypeIndex::TypeInfo
const CTypeInfo& CSurfaceTypeIndex::TypeInfo() const
{
	struct SurfaceEnums: DynArray<CEnumInfo::CEnumElem>
	{
		// Enumerate game surface types.
		SurfaceEnums()
		{
			CEnumInfo::CEnumElem elem = { 0, "", "" };

			// Empty elem for 0 value.
			push_back(elem);

			// Trigger surface types loading.
			gEnv->p3DEngine->GetMaterialManager()->GetDefaultLayersMaterial();

			// Get surface types.
			ISurfaceTypeEnumerator *pSurfaceTypeEnum = gEnv->p3DEngine->GetMaterialManager()->GetSurfaceTypeManager()->GetEnumerator();
			for (ISurfaceType *pSurfaceType = pSurfaceTypeEnum->GetFirst(); pSurfaceType; pSurfaceType = pSurfaceTypeEnum->GetNext())
			{
				elem.Value = pSurfaceType->GetId();
				elem.FullName = elem.ShortName = pSurfaceType->GetName();
				push_back(elem);
			}
		}
	};

	struct CCustomInfo: SurfaceEnums, CEnumInfo
	{
		CCustomInfo()
			: CEnumInfo("CSurfaceTypeIndex", sizeof(CSurfaceTypeIndex), size(), begin())
		{}
	};
	static CCustomInfo Info;
	return Info;
}

#if defined(TEST_TYPEINFO) && defined(_DEBUG)

struct STypeInfoTest
{
	STypeInfoTest()
	{
		TestTypes<UnitFloat8>(1.f);
		TestTypes<UnitFloat8>(0.5f);
		TestTypes<UnitFloat8>(37.f/255);
		TestTypes<UnitFloat8>(80.f/240);
		TestTypes<UnitFloat8>(1.001f);
		TestTypes<UnitFloat8>(-78.f);

		TestTypes<SFloat16>(0.f);
		TestTypes<SFloat16>(1.f);
		TestTypes<SFloat16>(0.999f);
		TestTypes<SFloat16>(0.9999f);
		TestTypes<SFloat16>(-123.4f);
		TestTypes<SFloat16>(-123.5f);
		TestTypes<SFloat16>(-123.6f);
		TestTypes<SFloat16>(-123.456f);
		TestTypes<SFloat16>(-0.00012345f);
		TestTypes<SFloat16>(-1e-8f);
		TestTypes<SFloat16>(1e27f);

		TestTypes<UFloat16>(1.f);
		TestTypes<UFloat16>(9.87654321f);
		TestTypes<UFloat16>(0.00012345f);
		TestTypes<UFloat16>(45678.9012f);
		TestTypes<UFloat16>(1e16f);

		TestTypes<Vec3U>(Vec3(2,3,4));
		TestTypes<Color3B>(Color3F(1,0.5,0.25));
	}
};
static STypeInfoTest _ParticleTypeInfoTest;

#endif
