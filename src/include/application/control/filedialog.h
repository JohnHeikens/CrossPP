#pragma once
#include "GlobalFunctions.h"
#include <Windows.h>

enum filedialogmode { open, save };

WCHAR* getopenfilename(filedialogmode mode, const WCHAR* filter, const WCHAR* title);