#ifndef __MetaUtils_h__
#define __MetaUtils_h__

#pragma once


namespace metautils
{
	// select
	template<bool Condition, typename Ty1, typename Ty2> struct select;
	
	template<typename Ty1, typename Ty2>
	struct select<true, Ty1, Ty2>
	{
		typedef Ty1 type;
	};

	template<typename Ty1, typename Ty2>
	struct select<false, Ty1, Ty2>
	{
		typedef Ty2 type;
	};

	// is_same_type
	template<typename Ty1, typename Ty2> struct is_same_type;
	
	template<typename Ty1>
	struct is_same_type<Ty1, Ty1>
	{
		enum { value = true, };
	};

	template<typename Ty1, typename Ty2> 
	struct is_same_type
	{
		enum { value = false, };
	};
};


#endif