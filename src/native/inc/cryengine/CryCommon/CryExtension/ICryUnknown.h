//////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2009.
// -------------------------------------------------------------------------
//  File name:   ICryUnknown.h
//  Version:     v1.00
//  Created:     02/25/2009 by CarstenW
//  Description: Part of CryEngine's extension framework.
// -------------------------------------------------------------------------
//
////////////////////////////////////////////////////////////////////////////

#ifndef _ICRYUNKNOWN_H_
#define _ICRYUNKNOWN_H_

#pragma once


#include "CryTypeID.h"
#include <BoostHelpers.h>


struct ICryFactory;
struct ICryUnknown;

namespace InterfaceCastSemantics
{

	template <class T>
	const CryInterfaceID& cryiidof()
	{
		return T::IID();
	}

#define _BEFRIEND_CRYIIDOF()\
	template <class T> friend const CryInterfaceID& InterfaceCastSemantics::cryiidof();


	template <class Dst, class Src>
	Dst* cryinterface_cast(Src* p)
	{
		return static_cast<Dst*>(p ? p->QueryInterface(cryiidof<Dst>()) : 0);
	}

	template <class Dst, class Src>
	Dst* cryinterface_cast(const Src* p)
	{
		return static_cast<const Dst*>(p ? p->QueryInterface(cryiidof<Dst>()) : 0);
	}

	template <class Dst, class Src>
	boost::shared_ptr<Dst> cryinterface_cast(const boost::shared_ptr<Src>& p)
	{
		Dst* dp = cryinterface_cast<Dst>(p.get());
		return dp ? boost::shared_ptr<Dst>(p, dp) : boost::shared_ptr<Dst>();
	}

#define _BEFRIEND_CRYINTERFACE_CAST()\
	template <class Dst, class Src> friend Dst* InterfaceCastSemantics::cryinterface_cast(Src*);\
	template <class Dst, class Src> friend Dst* InterfaceCastSemantics::cryinterface_cast(const Src*);\
	template <class Dst, class Src> friend boost::shared_ptr<Dst> InterfaceCastSemantics::cryinterface_cast(const boost::shared_ptr<Src>&);

} // namespace InterfaceCastSemantics

using InterfaceCastSemantics::cryiidof;
using InterfaceCastSemantics::cryinterface_cast;


#define _BEFRIEND_DELETER()\
	template <class T> friend void boost::checked_delete(T * x);


// prevent explicit destruction from client side (exception is boost::checked_delete which gets befriended)
#define _PROTECTED_DTOR(iname)\
protected:\
	virtual ~iname() {}


// Befriending cryinterface_cast<T>() via CRYINTERFACE_DECLARE is actually only needed for ICryUnknown
// since QueryInterface() is usually not redeclared in derived interfaces but it doesn't hurt either
#define CRYINTERFACE_DECLARE(iname, iidHigh, iidLow)\
	_BEFRIEND_CRYIIDOF()\
	_BEFRIEND_CRYINTERFACE_CAST()\
	_BEFRIEND_DELETER()\
	_PROTECTED_DTOR(iname)\
\
private:\
	static const CryInterfaceID& IID()\
	{\
		static const CryInterfaceID iid = {(uint64) iidHigh##LL, (uint64) iidLow##LL};\
		return iid;\
	}\
public:


struct ICryUnknown
{
	CRYINTERFACE_DECLARE(ICryUnknown, 0x1000000010001000, 0x1000100000000000)

	virtual ICryFactory* GetFactory() const = 0;

protected:
	virtual void* QueryInterface(const CryInterfaceID& iid) const = 0;
};

DECLARE_BOOST_POINTERS(ICryUnknown);


#endif // #ifndef _ICRYUNKNOWN_H_
