////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2010.
// -------------------------------------------------------------------------
//  File name:   IFlashUI.h
//  Version:     v1.00
//  Created:     10/9/2010 by Paul Reindell.
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////
#include DEVIRTUALIZE_HEADER_FIX(IFlashUI.h)

#ifndef __IFlashUI__h__
#define __IFlashUI__h__

#include <CryExtension/ICryUnknown.h>
#include <CryExtension/CryCreateClassInstance.h>
#include <IScaleformGFx.h>
#include <IFlowSystem.h>

#define IFlashUIExtensionName "FlashUI"

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////// UI variant data /////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

typedef NTypelist::CConstruct<
int,
float,
EntityId,
Vec3,
string,
wstring,
bool
>::TType TUIDataTypes;

//	Default conversion uses C++ rules.
template <class From, class To>
struct SUIConversion
{
	static ILINE bool ConvertValue( const From& from, To& to )
	{
		to = (To)from;
		return true;
	}
};

//	same type conversation
#define UIDATA_NO_CONVERSION(T) \
	template <> struct SUIConversion<T,T> { \
	static ILINE bool ConvertValue( const T& from, T& to ) { to = from; return true; } \
}
UIDATA_NO_CONVERSION(int);
UIDATA_NO_CONVERSION(float);
UIDATA_NO_CONVERSION(EntityId);
UIDATA_NO_CONVERSION(Vec3);
UIDATA_NO_CONVERSION(string);
UIDATA_NO_CONVERSION(wstring);
UIDATA_NO_CONVERSION(bool);
#undef FLOWSYSTEM_NO_CONVERSION

//	Specialization for converting to bool to avoid compiler warnings.
template <class From>
struct SUIConversion<From, bool>
{
	static ILINE bool ConvertValue( const From& from, bool& to )
	{
		to = (from != 0);
		return true;
	}
};

//	Strict conversation from float to int
template <>
struct SUIConversion<float, int>
{
	static ILINE bool ConvertValue( const float& from, int& to )
	{
		int tmp = (int) from;
		if ( fabs(from - (float) tmp) < FLT_EPSILON)
		{
			to = tmp;
			return true;
		}
		return false;
	}
};

//	Vec3 conversions...
template <class To>
struct SUIConversion<Vec3, To>
{
	static ILINE bool ConvertValue( const Vec3& from, To& to )
	{
		return SUIConversion<float, To>::ConvertValue( from.x, to );
	}
};

template <class From>
struct SUIConversion<From, Vec3>
{
	static ILINE bool ConvertValue( const From& from, Vec3& to )
	{
		float temp;
		if (!SUIConversion<From, float>::ConvertValue( from, temp ))
			return false;
		to.x = to.y = to.z = temp;
		return true;
	}
};

template <>
struct SUIConversion<Vec3, bool>
{
	static ILINE bool ConvertValue( const Vec3& from, bool& to )
	{
		to = from.GetLengthSquared() > 0;
		return true;
	}
};

//	String conversions...
#define UIDATA_STRING_CONVERSION(strtype,type,fmt,fct) \
	template <> \
struct SUIConversion<type, CryStringT<strtype> > \
{ \
	static ILINE bool ConvertValue( const type& from, CryStringT<strtype>& to ) \
{ \
	to.Format( fmt, from ); \
	return true; \
} \
}; \
	template <> \
struct SUIConversion<CryStringT<strtype>, type> \
{ \
	static ILINE bool ConvertValue( const CryStringT<strtype>& from, type& to ) \
{ \
	strtype *pEnd; \
	to = fct; \
	return from.size() > 0 && *pEnd == '\0'; \
} \
};

#define SINGLE_FCT(fct) (float) fct (from.c_str(),&pEnd)
#define DOUBLE_FCT(fct) fct (from.c_str(),&pEnd,10)

UIDATA_STRING_CONVERSION(char, int, "%d", DOUBLE_FCT(strtol) );
UIDATA_STRING_CONVERSION(char, float, "%f", SINGLE_FCT(strtod) );
UIDATA_STRING_CONVERSION(char, EntityId, "%u", DOUBLE_FCT(strtoul) );

UIDATA_STRING_CONVERSION(wchar_t, int, L"%d", DOUBLE_FCT(wcstol) );
UIDATA_STRING_CONVERSION(wchar_t, float, L"%f", SINGLE_FCT(wcstod) );
UIDATA_STRING_CONVERSION(wchar_t, EntityId, L"%u", DOUBLE_FCT(wcstoul) );

#undef UIDATA_STRING_CONVERSION
#undef SINGLE_FCT
#undef DOUBLE_FCT

template <>
struct SUIConversion<bool, string>
{
	static ILINE bool ConvertValue( const bool& from, string& to )
	{
		to.Format( "%d", from );
		return true;
	}	
};

template <>
struct SUIConversion<string, bool>
{
	static ILINE bool ConvertValue( const string& from, bool& to )
	{
		int to_i;
		if ( SUIConversion<string, int>::ConvertValue(from, to_i) )
		{
			to = !!to_i;
			return true;
		}
		if (0 == stricmp (from.c_str(), "true"))
		{
			to = true;
			return true;
		}
		if (0 == stricmp (from.c_str(), "false"))
		{
			to = false;
			return true;
		}
		return false;
	}	
};

template <>
struct SUIConversion<Vec3, string>
{
	static ILINE bool ConvertValue( const Vec3& from, string& to )
	{
		to.Format( "%f,%f,%f", from.x, from.y, from.z );
		return true;
	}
};

template <>
struct SUIConversion<string, Vec3>
{
	static ILINE bool ConvertValue( const string& from, Vec3& to )
	{
		return 3 == sscanf( from.c_str(), "%f,%f,%f", &to.x, &to.y, &to.z );
	}
};

template <>
struct SUIConversion<bool, wstring>
{
	static ILINE bool ConvertValue( const bool& from, wstring& to )
	{
		to.Format( L"%d", from );
		return true;
	}	
};

template <>
struct SUIConversion<wstring, bool>
{
	static ILINE bool ConvertValue( const wstring& from, bool& to )
	{
		int to_i;
		if ( SUIConversion<wstring, int>::ConvertValue(from, to_i) )
		{
			to = !!to_i;
			return true;
		}
		if (0 == wcsicmp (from.c_str(), L"true"))
		{
			to = true;
			return true;
		}
		if (0 == wcsicmp (from.c_str(), L"false"))
		{
			to = false;
			return true;
		}
		return false;
	}	
};

template <>
struct SUIConversion<Vec3, wstring>
{
	static ILINE bool ConvertValue( const Vec3& from, wstring& to )
	{
		to.Format( L"%f,%f,%f", from.x, from.y, from.z );
		return true;
	}
};

template <>
struct SUIConversion<wstring, Vec3>
{
	static ILINE bool ConvertValue( const wstring& from, Vec3& to )
	{
		return 3 == swscanf( from.c_str(), L"%f,%f,%f", &to.x, &to.y, &to.z );
	}
};

template <>
struct SUIConversion<string, wstring>
{
	static ILINE bool ConvertValue( const string& from, wstring& to )
	{
		size_t len = from.length();
		wchar_t* buff = new wchar_t[len + 1];
		buff[len] = '\0';
		mbstowcs(buff, from.c_str(), len);
		to = buff;
		delete[] buff;
		return true;
	}
};

template <>
struct SUIConversion<wstring, string>
{
	static ILINE bool ConvertValue( const wstring& from, string& to )
	{
		size_t len = from.length();
		char* buff = new char[len + 1];
		buff[len] = '\0';
		size_t res = wcstombs(buff, from.c_str(), len);
		to = buff;
		delete[] buff;
		return res == len;
	}
};


enum EUIDataTypes
{
	eUIDT_Any = -1,
	eUIDT_Int =				NTypelist::IndexOf<int, TUIDataTypes>::value,
	eUIDT_Float =			NTypelist::IndexOf<float, TUIDataTypes>::value,
	eUIDT_EntityId =	NTypelist::IndexOf<EntityId, TUIDataTypes>::value,
	eUIDT_Vec3 =			NTypelist::IndexOf<Vec3, TUIDataTypes>::value,
	eUIDT_String =		NTypelist::IndexOf<string, TUIDataTypes>::value,
	eUIDT_WString =		NTypelist::IndexOf<wstring, TUIDataTypes>::value,
	eUIDT_Bool =			NTypelist::IndexOf<bool, TUIDataTypes>::value,
};

typedef CConfigurableVariant<TUIDataTypes, sizeof(void*), SUIConversion> TUIData;


////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////// UI Arguments //////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
#define UIARGS_DEFAULT_DELIMITER "|"

struct SUIArguments
{
	SUIArguments() : m_cDelimiter( UIARGS_DEFAULT_DELIMITER ), m_Dirty(eBDF_None) {};
	template <class T>
	SUIArguments( const T* argStringList ) : m_cDelimiter( UIARGS_DEFAULT_DELIMITER ) { SetArguments( argStringList ); }
	SUIArguments( const SFlashVarValue* vArgs, int iNumArgs ) : m_cDelimiter( UIARGS_DEFAULT_DELIMITER) { SetArguments( vArgs, iNumArgs ); }
	SUIArguments( const TUIData& data ) : m_cDelimiter( UIARGS_DEFAULT_DELIMITER ) { AddArgument( data ); }

	template <class T>
	void SetArguments( const T* argStringList )
	{
		Clear();
		AddArguments( argStringList );
	}


	template <class T>
	void AddArguments( const T* argStringList )
	{
		CryStringT<T> delimiter_str = GetDelimiter<T>();
		CryStringT<T> str = argStringList;
		while ( str.length() > 0 ) 
		{
			typename CryStringT<T>::size_type loc = str.find( delimiter_str, 0 );
			if ( loc != CryStringT<T>::npos )
			{
				CryStringT<T> arg = str.substr( 0, loc );
				AddArgument( arg );
				str = str.substr( loc + delimiter_str.length() );
			}
			else
			{
				AddArgument( str );
				break;
			}
		}
		setStringBuffer( argStringList );
	}

	void SetArguments( const SFlashVarValue* vArgs, int iNumArgs )
	{
		Clear();
		AddArguments( vArgs, iNumArgs );
	}

	void AddArguments( const SFlashVarValue* vArgs, int iNumArgs )
	{
		for (int i = 0; i < iNumArgs; ++i)
		{
			switch( vArgs[i].GetType() )
			{
			case SFlashVarValue::eBool:					AddArgument( vArgs[i].GetBool() );									break;
			case SFlashVarValue::eInt:					AddArgument( vArgs[i].GetInt() );										break;
			case SFlashVarValue::eUInt:					AddArgument( vArgs[i].GetUInt() );									break;
			case SFlashVarValue::eFloat:				AddArgument( vArgs[i].GetFloat() );									break;
			case SFlashVarValue::eDouble:				AddArgument( (float) vArgs[i].GetDouble() );				break;
			case SFlashVarValue::eConstStrPtr:	AddArgument( string(vArgs[i].GetConstStrPtr()) );		break;
			case SFlashVarValue::eConstWstrPtr:	AddArgument( wstring(vArgs[i].GetConstWstrPtr()) );	break;
			case SFlashVarValue::eNull:					AddArgument( string("NULL") );											break;
			case SFlashVarValue::eObject:				AddArgument( string("OBJECT") );										break;
			case SFlashVarValue::eUndefined:		AddArgument( string("UNDEFINED") );									break;
			}
		}
	}

	template< class T >
	void AddArgument( const T& arg )
	{
		m_ArgList.push_back( TUIData( arg ) );
		m_Dirty = eBDF_ALL;
	}

	void Clear()
	{
		m_ArgList.clear();
		m_Dirty = eBDF_ALL;
	}

	template< class T >
	static SUIArguments Create(const T& arg)
	{
		SUIArguments args;
		args.AddArgument(arg);
		return args;
	}

	int GetArgCount() const { return m_ArgList.size(); }

	const char* GetAsString() const { return updateStringBuffer( m_sArgStringBuffer, eBDF_String ); }
	const wchar_t* GetAsWString() const { return updateStringBuffer( m_sArgWStringBuffer, eBDF_WString ); }
	const SFlashVarValue* GetAsList() const { return updateFlashBuffer(); }

	const TUIData& GetArg( int index ) const
	{
		if ( index >= 0 && index < m_ArgList.size() )
			return m_ArgList[index];
		static TUIData undef( string( "undefined" ) );
		return undef;
	}

	template < class T >
	bool GetArg( int index, T &val ) const
	{
		if ( index >= 0 && index < m_ArgList.size() )
			return m_ArgList[index].GetValueWithConversion( val );
		return false;
	}

	void SetDelimiter( const string& delimiter ) 
	{ 
		if ( delimiter != m_cDelimiter )
		{
			m_Dirty |= eBDF_String | eBDF_WString;
		}
		m_cDelimiter = delimiter; 
	}

private:
	DynArray< TUIData > m_ArgList;
	mutable string m_sArgStringBuffer;	// buffer for const char* GetAsString()
	mutable wstring m_sArgWStringBuffer;	// buffer for const wchar_t* GetAsWString()
	mutable DynArray< SFlashVarValue > m_FlashValueBuffer; // buffer for const SFlashVarValue* GetAsList()
	string m_cDelimiter;
	
	enum EBufferDirtyFlag
	{
		eBDF_None 		= 0x0,
		eBDF_String 	= 0x1,
		eBDF_WString 	= 0x2,
		eBDF_FlashVar	= 0x4,
		eBDF_ALL 			= 0x7
	};
	mutable uint m_Dirty;

	inline SFlashVarValue* updateFlashBuffer() const
	{
		if (m_Dirty & eBDF_FlashVar)
		{
			m_Dirty &= ~eBDF_FlashVar;
			m_FlashValueBuffer.clear();
			for (DynArray< TUIData >::const_iterator it = m_ArgList.begin(); it != m_ArgList.end(); ++it)
			{
				bool bConverted = false;
				switch (it->GetType())
				{
				case eUIDT_Bool:
					AddValue<bool>(*it);
					break;
				case eUIDT_Int:
					AddValue<int>(*it);
					break;
				case eUIDT_EntityId:
					AddValue<EntityId>(*it);
					break;
				case eUIDT_Float:
					{
						bool bRes = TryAddValue<int>(*it)
							|| AddValue<float>(*it);
							assert(bRes);
					}
					break;
				case eUIDT_String:
					{
						bool bRes = TryAddValue<int>(*it)
							|| TryAddValue<float>(*it)
							|| AddValue<string>(*it);
							assert(bRes);
					}
					break;
				case eUIDT_WString:
					{
						bool bRes = TryAddValue<int>(*it)
							|| TryAddValue<float>(*it)
							|| AddValue<wstring>(*it);
							assert(bRes);
					}
					break;
				case eUIDT_Any:
				default:
					assert(false);	// not supported since string->wstring or vice versa conversation 
													// would need to make a copy of data ptr and buffer somewhere. 
													// SFlashVarValue only stores char/wchar_t* ptr
					break;
				}
			}
		}
		return m_FlashValueBuffer.size() > 0 ? &m_FlashValueBuffer[0] : NULL;
	}

	template < class T >
	inline bool AddValue( const TUIData& data ) const
	{
		const T* val = data.GetPtr<T>();
		assert(val);
		m_FlashValueBuffer.push_back( SFlashVarValue(*val) );
		return true;
	}

	template < class T >
	inline bool TryAddValue( const TUIData& data ) const
	{
		T val;
		if ( data.GetValueWithConversion( val ) )
		{
			m_FlashValueBuffer.push_back( SFlashVarValue(val) );
			return true;
		}
		return false;
	}

	template <class T>
	inline const T* updateStringBuffer(CryStringT<T>& buffer, uint flag) const
	{
		if (m_Dirty & flag)
		{
			m_Dirty &= ~flag;
			CryStringT<T> delimiter_str = GetDelimiter<T>();
			buffer.clear();
			for ( DynArray< TUIData >::const_iterator it = m_ArgList.begin(); it != m_ArgList.end(); ++it )
			{
				if (buffer.size() > 0) buffer += delimiter_str;
				CryStringT<T> val;
				bool bRes = it->GetValueWithConversion(val);
				assert(bRes && "try to convert to char* string list but some of the values are unsupported wchar_t*");
				buffer += val;
			}
		}
		return buffer.c_str();
	}

	template <class T>
	inline const CryStringT<T> GetDelimiter() const
	{
		TUIData delimiter(m_cDelimiter);
		CryStringT<T> delimiter_str;
		delimiter.GetValueWithConversion(delimiter_str);
		return delimiter_str;
	}

	template <class T>
	inline void setStringBuffer(const T* str) {}
};

// Specialize in global scope
template <>
inline const CryStringT<char> SUIArguments::GetDelimiter() const
{
	return m_cDelimiter;
}

template <>
inline void SUIArguments::setStringBuffer(const char* str)
{
	m_sArgStringBuffer = str;
	m_Dirty &= ~eBDF_String;
}

template <>
inline void SUIArguments::setStringBuffer(const wchar_t* str)
{
	m_sArgWStringBuffer = str;
	m_Dirty &= ~eBDF_WString;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////// UI Descriptions /////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
struct SUIParameterDesc
{
	SUIParameterDesc() : sName("undefined"), sDisplayName("undefined"), sDesc("undefined") {} 
	SUIParameterDesc( string name, string displ, string desc) : sName(name), sDisplayName(displ), sDesc(desc) {}
	string sName;
	string sDisplayName;
	string sDesc;

	inline bool operator==( const SUIParameterDesc& other ) const
	{
		return sName == other.sName && sDisplayName == other.sDisplayName;
	}
};
typedef DynArray< SUIParameterDesc > TUIParams;

struct SUIEventDesc : public SUIParameterDesc
{
	SUIEventDesc() : IsDynamic(false) {}
	SUIEventDesc( string name, string displ, string desc, bool isDyn = false ) : SUIParameterDesc( name, displ, desc ), IsDynamic(isDyn) {}
	TUIParams Params;
	bool IsDynamic;

	inline bool operator==( const SUIEventDesc& other ) const
	{		
		bool res = sName == other.sName && sDisplayName == other.sDisplayName && IsDynamic == other.IsDynamic && Params.size() == other.Params.size();
		for ( int i = 0; i < Params.size() && res; ++i)
		{
			res &= Params[i] == other.Params[i];
		}
		return res;
	}
};
typedef DynArray< SUIEventDesc > TUIEvents;


////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// Dyn Texture Interface /////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

struct IUITexture
{
	virtual void SetFlashPlayer( IFlashPlayer* pFlashPlayer ) = 0;
	virtual void SetVisible( bool bVisible ) = 0;
	virtual void ElementRemoved() = 0;

protected:
  virtual ~IUITexture() {}; 
};


////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////// UI Element ///////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
struct IUIElementEventListener
{
	virtual void OnUIEvent( const SUIEventDesc& event, const SUIArguments& args ) {};
	virtual void OnUnload() {};
protected:
  virtual ~IUIElementEventListener() {}; 
};

struct IUIElement;
UNIQUE_IFACE struct IUIElementIterator
{
	virtual ~IUIElementIterator() {}

	virtual void AddRef() = 0;
	virtual void Release() = 0;
	virtual IUIElement* Next() = 0;
};
TYPEDEF_AUTOPTR(IUIElementIterator);
typedef IUIElementIterator_AutoPtr IUIElementIteratorPtr;

UNIQUE_IFACE struct IUIElement
{
	struct SUIConstraints
	{
		enum EPositionType
		{
			ePT_Fixed,
			ePT_Fullscreen,
			ePT_Dynamic
		};

		enum EPositionAlign
		{
			ePA_Lower,
			ePA_Mid,
			ePA_Upper,
		};

		SUIConstraints() 
			: eType(ePT_Dynamic)
			, iLeft(0)
			, iTop(0)
			, iWidth(1024)
			, iHeight(768)
			, eHAlign(ePA_Mid)
			, eVAlign(ePA_Mid)
			, bScale(true)
		{
		}

		SUIConstraints( EPositionType type, int left, int top, int width, int height, EPositionAlign halign, EPositionAlign valign, bool scale )
			: eType(type)
			, iLeft(left)
			, iTop(top)
			, iWidth(width)
			, iHeight(height)
			, eHAlign(halign)
			, eVAlign(valign)
			, bScale(scale)
		{
		}

		EPositionType eType;
		int iLeft;
		int iTop;
		int iWidth;
		int iHeight;
		EPositionAlign eHAlign;
		EPositionAlign eVAlign;
		bool bScale;
	};

	enum EFlashUIFlags
	{
		eFUI_HARDWARECURSOR = 0x001,
		eFUI_MOUSEEVENTS	= 0x002,
		eFUI_KEYEVENTS		= 0x004,
		eFUI_CONSOLE_MOUSE	= 0x008,
		eFUI_CONSOLE_CURSOR = 0x010,
		eFUI_CONTROLLER_INPUT = 0x020,
		eFUI_EVENTS_EXCLUSIVE = 0x040,
	};

	enum EControllerInputEvent
	{
		eCIE_Up = 0,
		eCIE_Down,
		eCIE_Left,
		eCIE_Right,
		eCIE_Action,
		eCIE_Back,
	};

	enum EControllerInputState
	{
		eCIS_OnPress = 0,
		eCIS_OnRelease,
	};

	virtual ~IUIElement() {}

	// instances
	virtual uint GetInstanceID() const = 0;
	virtual IUIElement* GetInstance( uint instanceID ) = 0;
	virtual IUIElementIteratorPtr GetInstances() const = 0;

	// common
	virtual void SetName( const char* name ) = 0;
	virtual const char* GetName() const = 0;

	virtual void SetGroupName( const char* groupName ) = 0;
	virtual const char* GetGroupName() const = 0;

	virtual void SetFlashFile( const char* flashFile ) = 0;
	virtual const char* GetFlashFile() const = 0;

	virtual bool Init( bool bLoadAsset = true ) = 0;
	virtual void Unload() = 0; 
	virtual void Reload() = 0;
	virtual void Update( float fDeltaTime ) = 0;
	virtual void Render() = 0;
	virtual void RenderLockless() = 0;

	// visibility
	virtual void RequestHide() = 0;

	virtual void SetVisible( bool bVisible ) = 0;
	virtual bool IsVisible() const = 0;

	virtual void SetFlag( EFlashUIFlags flag, bool bSet ) = 0;
	virtual bool HasFlag( EFlashUIFlags flag ) const = 0;

	virtual float GetAlpha() const = 0;
	virtual void SetAlpha( float fAlpha ) = 0;

	virtual int GetLayer() const = 0;
	virtual void SetLayer( int iLayer ) = 0;

	virtual void SetConstraints( const SUIConstraints& newConstraints ) = 0;
	virtual const IUIElement::SUIConstraints& GetConstraints() const = 0;

	// raw IFlashPlayer
	virtual IFlashPlayer* GetFlashPlayer() = 0;

	// definitions
	virtual const SUIParameterDesc* GetVariableDesc( int index ) const = 0;
	virtual const SUIParameterDesc* GetVariableDesc( const char* varName ) const = 0;
	virtual int GetVariableCount() const = 0;

	virtual const SUIParameterDesc* GetArrayDesc( int index ) const = 0;
	virtual const SUIParameterDesc* GetArrayDesc( const char* arrayName ) const = 0;
	virtual int GetArrayCount() const = 0;

	virtual const SUIParameterDesc* GetMovieClipDesc( int index ) const = 0;
	virtual const SUIParameterDesc* GetMovieClipDesc( const char* movieClipName ) const = 0;
	virtual int GetMovieClipCount() const = 0;

	virtual const SUIEventDesc* GetEventDesc( int index ) const = 0;
	virtual const SUIEventDesc* GetEventDesc( const char* eventName ) const = 0;
	virtual int GetEventCount() const = 0;

	virtual const SUIEventDesc* GetFunctionDesc( int index ) const = 0;
	virtual const SUIEventDesc* GetFunctionDesc( const char* functionName ) const = 0;
	virtual int GetFunctionCount() const = 0;

	virtual void UpdateViewPort() = 0;

	virtual bool Serialize( XmlNodeRef& xmlNode, bool bIsLoading ) = 0;

	// event listener
	virtual void AddEventListener( IUIElementEventListener* pListener ) = 0;
	virtual void RemoveEventListener( IUIElementEventListener* pListener ) = 0;

	// functions and objects
	virtual bool CallFunction( const char* fctName, const SUIArguments& args = SUIArguments(), TUIData* pDataRes = NULL ) = 0;
	virtual bool CallFunction( const SUIEventDesc* pFctDesc, const SUIArguments& args = SUIArguments(), TUIData* pDataRes = NULL ) = 0;

	virtual IFlashVariableObject* GetMovieClip( const char* movieClipName ) = 0;
	virtual IFlashVariableObject* GetMovieClip( const SUIParameterDesc* pMovieClipDesc ) = 0;

	virtual bool SetVariable( const char* varName, const TUIData& value ) = 0;
	virtual bool SetVariable( const SUIParameterDesc* pVarDesc, const TUIData& value ) = 0;

	virtual bool GetVariable( const char* varName, TUIData& valueOut ) = 0;
	virtual bool GetVariable( const SUIParameterDesc* pVarDesc, TUIData& valueOut ) = 0;

	virtual bool SetArray( const char* arrayName, const SUIArguments& values ) = 0;
	virtual bool SetArray( const SUIParameterDesc* pArrayDesc, const SUIArguments& values ) = 0;

	virtual bool GetArray( const char* arrayName, SUIArguments& valuesOut ) = 0;
	virtual bool GetArray( const SUIParameterDesc* pArrayDesc, SUIArguments& valuesOut ) = 0;

	template <class T>
	bool SetVar( const char* varName, const T& value)
	{
		return SetVariable( varName, TUIData(value) );
	}

	template <class T>
	T GetVar( const char* varName )
	{
		TUIData out;
		if ( GetVariable( varName, out ) )
		{
			T res;
			if ( out.GetValueWithConversion( res ) )
				return res;
		}
		assert(false);
		return T();
	}

	// ITexture 
	virtual void LoadTexIntoMc( const char* movieClip, ITexture* pTexture ) = 0;
	virtual void LoadTexIntoMc( const SUIParameterDesc* pMovieClipDesc, ITexture* pTexture ) = 0;

	// dynamic textures
	virtual void AddTexture( IUITexture* pTexture ) = 0;
	virtual void RemoveTexture( IUITexture* pTexture ) = 0;

	// input events
	virtual void SendCursorEvent( SFlashCursorEvent::ECursorState evt, int iX, int iY ) = 0;
	virtual void SendKeyEvent( SFlashKeyEvent evt ) = 0;
	virtual void SendControllerEvent( EControllerInputEvent event, EControllerInputState state ) = 0;

	virtual void GetMemoryUsage(ICrySizer * s) const = 0;
};
typedef std::vector< IUIElement* > TUIElements;

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////// UI Action ///////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
UNIQUE_IFACE struct IUIAction
{
	virtual ~IUIAction() {}

	virtual const char* GetName() const = 0;
	virtual void SetName( const char* name ) = 0;

	virtual bool Init() = 0;

	virtual IFlowGraphPtr GetFlowGraph() const = 0;

	virtual bool Serialize( XmlNodeRef& xmlNode, bool bIsLoading ) = 0;

	virtual void GetMemoryUsage(ICrySizer * s) const = 0;
};
typedef std::vector< IUIAction* > TUIActions;

struct IUIActionListener
{
	virtual void OnStart( IUIAction* pAction, const SUIArguments& args ) = 0;
	virtual void OnEnd( IUIAction* pAction, const SUIArguments& args ) = 0;
protected:
  virtual ~IUIActionListener() {}
};

UNIQUE_IFACE struct IUIActionManager
{
	virtual ~IUIActionManager() {}

	virtual void StartAction( IUIAction* pAction, const SUIArguments& args ) = 0;
	virtual void EndAction( IUIAction* pAction, const SUIArguments& args ) = 0;
	virtual void EnableAction( IUIAction* pAction, bool bEnable ) = 0;

	virtual void AddListener( IUIActionListener* pListener ) = 0;
	virtual void RemoveListener( IUIActionListener* pListener ) = 0;

	virtual void GetMemoryUsage(ICrySizer * s) const = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////// UI Events ///////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
struct SUIEvent
{
	SUIEvent( uint evt, SUIArguments agruments ) : event(evt), args(agruments) {}
	uint event;
	SUIArguments args;
};

struct IUIEventListener
{
	virtual void OnEvent( const SUIEvent& event ) = 0;
protected:
  virtual ~IUIEventListener() {}
};

UNIQUE_IFACE struct IUIEventSystem
{
	enum EEventSystemType
	{
		eEST_UI_TO_SYSTEM,
		eEST_SYSTEM_TO_UI,
	};

	virtual ~IUIEventSystem() {}

	virtual const char* GetName() const = 0;
	virtual IUIEventSystem::EEventSystemType GetType() const = 0;

	virtual uint RegisterEvent( const SUIEventDesc& eventDesc ) = 0;

	virtual void RegisterListener( IUIEventListener* pListener ) = 0;
	virtual void UnregisterListener( IUIEventListener* pListener ) = 0;

	virtual void SendEvent( const SUIEvent& event ) = 0;

	virtual const SUIEventDesc* GetEventDesc( int index ) const = 0;
	virtual const SUIEventDesc* GetEventDesc( const char* eventName ) const = 0;
	virtual int GetEventCount() const = 0;
	
	virtual uint GetEventId( const char* sEventName ) = 0;

	virtual void GetMemoryUsage(ICrySizer * s) const = 0;
};

UNIQUE_IFACE struct IUIEventSystemIterator
{
	virtual ~IUIEventSystemIterator() {}

	virtual void AddRef() = 0;
	virtual void Release() = 0;
	virtual IUIEventSystem* Next( string &name ) = 0;
};

TYPEDEF_AUTOPTR(IUIEventSystemIterator);
typedef IUIEventSystemIterator_AutoPtr IUIEventSystemIteratorPtr;

template<class T>
struct SUIEventHelper
{
	typedef void (T::*TEventFct) ( const SUIEvent& event );
	void RegisterEvent( IUIEventSystem* pEventSystem, const SUIEventDesc &event, TEventFct fct )
	{
		mFunctionMap[pEventSystem->RegisterEvent(event)] = fct;
	}
	void Dispatch( T* pThis, const SUIEvent& event )
	{
		TFunctionMapIter it = mFunctionMap.find( event.event );
		if (it != mFunctionMap.end())
			(pThis->*it->second)( event );
	}

private:
	typedef std::map<uint, TEventFct> TFunctionMap;
	typedef typename TFunctionMap::iterator TFunctionMapIter;
	TFunctionMap mFunctionMap;
};


////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// UI Interface ///////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
UNIQUE_IFACE struct IFlashUI : public ICryUnknown
{
	CRYINTERFACE_DECLARE( IFlashUI, 0xE1161004DA5B4F04, 0x9DFF8FC0EACE3BD4 )

public:
	DEVIRTUALIZATION_VTABLE_FIX

	// init the Flash UI system
	virtual void Init() = 0;
	virtual bool PostInit() = 0;

	// update the ui system
	virtual void Update(float fDeltatime) = 0;

	// reload UI xml files
	virtual void Reload() = 0;

	// shut down
	virtual void ShutDown() = 0;

	virtual bool LoadElementsFromFile( const char* fileName ) = 0;
	virtual bool LoadActionFromFile( const char* fileName ) = 0;

	// access for IUIElements
	virtual IUIElement* GetUIElement( const char* name ) const = 0;
	virtual IUIElement* GetUIElement( int index ) const = 0;
	virtual int GetUIElementCount() const = 0;

	virtual IUIElement* GetUIElementByInstanceStr( const char* UIInstanceStr) const = 0;

	// access for IUIActions
	virtual IUIAction* GetUIAction( const char* name ) const = 0;
	virtual IUIAction* GetUIAction( int index ) const = 0;
	virtual int GetUIActionCount() const = 0;

	virtual IUIActionManager* GetUIActionManager() const = 0;
	virtual void UpdateFG() = 0;

	// event system to auto create flownodes for communication between flash and c++
	virtual IUIEventSystem* CreateEventSystem( const char* name, IUIEventSystem::EEventSystemType eType ) = 0;
	virtual IUIEventSystem* GetEventSystem( const char* name, IUIEventSystem::EEventSystemType eType ) = 0;
	virtual IUIEventSystemIteratorPtr CreateEventSystemIterator( IUIEventSystem::EEventSystemType eType ) = 0; 

	virtual void DispatchControllerEvent( IUIElement::EControllerInputEvent event, IUIElement::EControllerInputState state ) = 0;

	virtual void CheckPreloadedTexture(ITexture* pTexture) const = 0;

	virtual void GetMemoryStatistics(ICrySizer * s) const = 0;
};

DECLARE_BOOST_POINTERS( IFlashUI );

static IFlashUIPtr GetIFlashUIPtr()
{
	IFlashUIPtr pFlashUI;
	if (gEnv && gEnv->pSystem)
		CryCreateClassInstance(IFlashUIExtensionName, pFlashUI);
	return pFlashUI;
}

#endif