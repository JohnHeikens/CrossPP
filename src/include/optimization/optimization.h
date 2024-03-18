#pragma once
#include <assert.h>
#include <stdexcept>
#include "GlobalFunctions.h"
#include "array/wstring.h"
#include "macrotricks.h"
#include <string>

#ifdef _DEBUG
//#define EXTRADEBUGCHECKS
#ifdef EXTRADEBUGCHECKS
//#if defined(_DEBUG) && defined(EXTRADEBUGCHECKS)
# define assumeInRelease1(shouldBeTrue)   throwIfFalse(shouldBeTrue)
# define assumeInRelease2(shouldBeTrue, errorMessageIfFalse)   throwIfFalse((shouldBeTrue), (errorMessageIfFalse))
#else//no checks, no nothing
# define assumeInRelease1(shouldBeTrue)
# define assumeInRelease2(shouldBeTrue, errorMessageIfFalse)
#endif
#else
#if _MSC_VER
# define assumeInRelease1(shouldBeTrue)   __assume(shouldBeTrue)
# define assumeInRelease2(shouldBeTrue, errorMessageIfFalse)   __assume(shouldBeTrue)
#else
# define assumeInRelease1(shouldBeTrue)   [[assume(shouldBeTrue)]]
# define assumeInRelease2(shouldBeTrue, errorMessageIfFalse)   [[assume(shouldBeTrue)]]
#endif
#endif



#define assumeInRelease(...) GetMacroOverLoad(__VA_ARGS__, noFunctionFound, assumeInRelease2, assumeInRelease1)(__VA_ARGS__)

//https://docs.microsoft.com/en-us/cpp/intrinsics/assume?view=msvc-170

//#define assumeInRelease(e) (((e) || assert(__FILE__, __LINE__)), [[assume(e)]] )

inline void throwIfFalse(cbool& shouldBeTrue, std::wstring errorMessageIfFalse = L"")
{
	if (!shouldBeTrue)
	{
		throw std::runtime_error(WStringToString(errorMessageIfFalse));
	}
}


//for if the compiler gets smarter.

/*constexpr __forceinline void assumeInRelease(cbool& shouldBeTrue, std::wstring errorMessageIfFalse = L"")
{
#ifdef _DEBUG
	if (!shouldBeTrue)
	{
		throw "should not be able to come here";
	}
#else
	[[assume(shouldBeTrue)]];
#endif // _DEBUG
}*/