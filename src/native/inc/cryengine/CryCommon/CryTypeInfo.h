//////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2007.
// -------------------------------------------------------------------------
//  File name:   TypeInfo.h
//  Version:     v1.00
//  Created:     03/05/2005 by Scott.
//  Description: Declaration of CTypeInfo and related types.
// -------------------------------------------------------------------------
//
////////////////////////////////////////////////////////////////////////////

#ifndef __CRY_TYPEINFO_H
#define __CRY_TYPEINFO_H
#pragma once

#include <platform.h>
#include "CryArray.h"
#include "TypeInfo_decl.h"

class ICrySizer;

#ifdef __SNC__
  #pragma control %push diag
	#pragma control diag=0
#endif

class CCryName;
string ToString(CCryName const& val);
bool FromString(CCryName& val, const char* s);

//---------------------------------------------------------------------------
// Helper classes for type info

#define OPT_STRUCT(Struc) \
	typedef Struc TThis; \
	Struc(int init = 0) { memset(this, -init, sizeof(*this)); }

#define VAR_OPT(Type, Var) \
	Type _##Var; \
	TThis Var(Type const& v) const { TThis t = *this; t._##Var = v; return t; }

#define BIT_OPT(Var) \
	int _##Var: 1; \
	TThis Var(int v = 1) const { TThis t = *this; t._##Var = v; return t; }

// Specify options for converting data to/from strings
struct FToString
{
	OPT_STRUCT(FToString)
	BIT_OPT(SkipDefault)		// Omit default values on writing.
	BIT_OPT(TruncateSub)		// Remove trailing empty sub-vaules.
	BIT_OPT(Sub)						// Write sub-structures (internal usage).
};

struct FFromString
{
	OPT_STRUCT(FFromString)
	BIT_OPT(SkipEmpty)			// Do not set values from empty strings (otherwise, set to zero).
	BIT_OPT(Finalize)				// Optimize data after reading, allow no more write access.
};

struct FStringConversion: FToString, FFromString
{
	FStringConversion(FToString f)
		: FToString(f) {}
	FStringConversion(FFromString f)
		: FFromString(f) {}
};

// Specify which limits a variable has
enum ENumericLimit
{
	eLimit_Min,
	eLimit_Max,
	eLimit_SoftMin,
	eLimit_SoftMax,
	eLimit_MinIsInfinite,
	eLimit_Step,
};

//---------------------------------------------------------------------------
// Type info base class, and default implementation

struct CTypeInfo
{
	virtual ~CTypeInfo(){}
	cstr		Name;
	size_t	Size;

	CTypeInfo( cstr name, size_t size )
		: Name(name), Size(size) {}

	//
	// Inheritance.
	//
	virtual bool IsType( CTypeInfo const& Info ) const		
		{ return this == &Info; }

	template<class T> bool IsType() const																		
		{ return IsType(TypeInfo((T*)0)); }

	//
	// Data access interface.
	//

	// Convert value to string.
	virtual string ToString(const void* data, FToString flags = 0, const void* def_data = 0) const
		{ return ""; }

	// Write value from string, return success.
	virtual bool FromString(void* data, cstr str, FFromString flags = 0) const
		{ return false; }

	// Read and write values of a specified type.
	virtual bool ToValue(const void* data, void* value, const CTypeInfo& typeVal) const
		{ return false; }

	virtual bool FromValue(void* data, const void* value, const CTypeInfo& typeVal) const
		{ return false; }

	// Templated interface to above functions.
	template<class T> bool ToValue(const void* data, T& value) const
		{ return ToValue(data, &value, TypeInfo(&value)); }
	template<class T> bool FromValue(void* data, const T& value) const
		{ return FromValue(data, &value, TypeInfo(&value)); }

	virtual bool ValueEqual(const void* data, const void* def_data = 0) const
		{ return ToString(data, FToString().SkipDefault().TruncateSub(), def_data).empty(); }

	virtual bool GetLimit(ENumericLimit eLimit, float& fVal) const
		{ return false; }

	// Convert numeric formats from big-to-little endian or vice versa.
	// Swaps bitfield order as well (which may be separate from integer bit order).
	virtual void SwapEndian(void* pData, size_t nCount, bool bWriting) const;

	// Track memory used by any internal structures (not counting object size itself).
	// Add to CrySizer as needed, return remaining mem count.
	virtual void GetMemoryUsage(ICrySizer* pSizer, const void* data) const
		{}

	//
	// Structure interface.
	//
	struct CVarInfo
	{
		uint32							Offset;							// Offset in bytes from struct start.
		const CTypeInfo&		Type;								// Info for type of variable.
		uint32							ArrayDim: 20,				// Number of array elements, or bits if bitfield.			
												bBitfield: 1,				// Var is a bitfield, ArrayDim is number of bits.
												BitOffset: 6,				// Additional offset in bits for bitfields.
																						// Bit offset is computed in declaration order; on some platforms, it goes high to low.
												BitWordWidth: 2,		// Width of bitfield = 1 byte << BitWordWidth
												bBaseClass: 1,			// Sub-var is actually a base class.
												bUnionAlias: 1;			// If 2nd or greater element of a union.
		cstr								Name;								// Display name of variable.
		cstr								Attrs;							// Var-specific attribute string, of form: 
																						// "<name=value>" for each attr, concatenated.
																						// Remaining text considered as comment.

		// Accessors.
		cstr GetName() const									{ assert(*Name || bBaseClass); return Name; }
		size_t GetDim() const									{ return bBitfield ? 1 : ArrayDim; }
		size_t GetSize() const								
		{ 
			if (bBitfield)
				return (size_t)1 << BitWordWidth;
			else if (bBaseClass && Type.Size > 0 && !Type.HasSubVars())
				// Handle anomalous case of zero-size base class that compiler misreports as size 1.
				return 0;
			else
				return Type.Size * ArrayDim; 
		}
		size_t GetElemSize() const						{ return bBitfield? (size_t)1 << BitWordWidth : Type.Size; }
		size_t GetBits() const								{ return bBitfield ? ArrayDim : ArrayDim * Type.Size * 8; }
		bool IsBaseClass() const							{ return bBaseClass; }

		bool GetLimit(ENumericLimit eLimit, float& fVal) const
		{
			return Type.GetLimit(eLimit, fVal);
		}

		// Useful functions.
		void* GetAddress(void* base) const
		{
			return (char*)base + Offset;
		}
		const void* GetAddress(const void* base) const
		{
			return (const char*)base + Offset;
		}
		bool FromString(void* base, cstr str, FFromString flags = 0) const
		{
			assert(!bBitfield);
			return Type.FromString( (char*)base + Offset, str, flags );
		}
		string ToString(const void* base, FToString flags = 0, const void* def_base = 0) const
		{
			assert(!bBitfield);
			return Type.ToString( (const char*)base + Offset, flags, def_base ? (const char*)def_base + Offset : 0 );
		}

		// Attribute access. Not fast.
		bool GetAttr(cstr name) const;
		bool GetAttr(cstr name, float& val) const;
		bool GetAttr(cstr name, string& val) const;

		// Comment, excluding attributes.
		cstr GetComment() const;
	};

	// Structure var iteration.
	virtual const CVarInfo* NextSubVar(const CVarInfo* pPrev) const		{ return 0; }
	inline bool HasSubVars() const  { return NextSubVar(0) != 0; }
	#define AllSubVars( pVar, Info ) (const CTypeInfo::CVarInfo* pVar = 0; pVar = (Info).NextSubVar(pVar); )

	// Named var search.
	virtual const CVarInfo* FindSubVar(cstr name) const  { return 0; }

	//
	// String enumeration interface.
	// Return sequential strings in enumeration, then 0 when out of range.
	// String/int conversion is handled by ToString/FromString.
	//
	virtual cstr EnumElem(uint nIndex) const { return 0; }
};

#ifdef __SNC__
	#pragma control %pop diag
#endif

#endif // __CRY_TYPEINFO_H
