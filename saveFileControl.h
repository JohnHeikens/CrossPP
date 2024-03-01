#pragma once
#include "gameTile.h"
#include "include/application/control/picturebox.h"
#include "include/application/control/label.h"
struct saveFileControl : gameTile
{
	saveFileControl(const std::wstring& saveFileName);
};