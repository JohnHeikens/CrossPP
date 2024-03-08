#include "GlobalFunctions.h"
#pragma once
enum mouseButton : byte {
	left,
	right,
	cancel,
	middle,
	count
};
constexpr vk mouseButtonToKeyCode(const mouseButton& button) {
	return (vk)button + VK_LBUTTON;
}
constexpr mouseButton keyCodeToMouseButton(cvk& keyCode) {
	return (mouseButton)(keyCode - VK_LBUTTON);
}