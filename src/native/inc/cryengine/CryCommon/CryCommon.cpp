// CryCommon.cpp 
//		This contains compiled code that is used by other projects in the solution.
//		Because we don't want static DLL dependencies, the CryCommon project is not compiled into a library.
//		Instead, this .cpp file is included in every project which needs it.
//		But we also include it in the CryCommon project (disabled in the build), 
//		so that CryCommon can have the same editable settings as other projects.

// Set this to 1 to get an output of some pre-defined compiler symbols.














































#include "TypeInfo_impl.h"
#include "CryTypeInfo.cpp"
#include "LCGRandom.cpp"
#include "MTPseudoRandom.cpp"
#include "CryStructPack.cpp"
#if !defined __CRYCG__
	#if defined(PS3) || defined(LINUX)
		#include "WinBase.cpp"
	#endif
#endif







#ifdef __GNUC__
// GCC+STLPORT bug workaround, see comment in ISerialize.h.
#include <ISerialize.h>
const uint16 SNetObjectID::InvalidId = ~uint16(0);
#endif

