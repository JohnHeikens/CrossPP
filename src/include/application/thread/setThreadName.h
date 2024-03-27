#include "GlobalFunctions.h"
#if onWindows
#include <Windows.h>
#elif onLinux
#include <sys/prctl.h>
#include "include/array/wstring.h"
#endif
#include <string>
/// @brief
/// @param threadName
/// @return true if the thread name was set successfully
inline bool setCurrentThreadName(const std::wstring &threadName)
{
#if onWindows
    HRESULT hr = SetThreadDescription(GetCurrentThread(), threadName.c_str());
    return !FAILED(hr);
#elif onLinux
    const std::string &threadNameString = WStringToString(threadName);
    prctl(PR_SET_NAME, threadName, 0, 0, 0);
#endif
}