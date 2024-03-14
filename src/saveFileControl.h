#pragma once
#include "gameTile.h"
#include "application/control/picturebox.h"
#include "application/control/label.h"
struct saveFileControl : gameTile
{
	saveFileControl(const std::wstring& saveFileName);
};