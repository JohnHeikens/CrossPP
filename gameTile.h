#pragma once

#include "include/application/control/form.h"
#include "include/application/control/picturebox.h"
#include "include/application/control/label.h"
struct gameTile : form
{
	gameTile(const std::wstring& saveFileName);
	pictureBox* screenShot = nullptr;
	label* infoLabel;
	std::wstring gameName = std::wstring(L"");
	virtual ~gameTile() override;
	virtual void layout(crectanglei2& newRect) override;
};