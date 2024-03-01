#include "GlobalFunctions.h"
#pragma once
enum mouseButton : byte {
	left = 0,
	right = 1,
	middle = 2,
	count
};
constexpr vk mouseButtonKeyCode[(byte)mouseButton::count]{
	VK_LBUTTON,
	VK_RBUTTON,
	VK_MBUTTON
};
constexpr mouseButton keyCodeToButton(cvk& keyCode) {
	return (mouseButton)(keyCode - VK_LBUTTON);
}