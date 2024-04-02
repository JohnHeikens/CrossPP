#pragma once
//#include "windowsIncluder.h"
//inline bool cursorVisible()
//{
//	CURSORINFO ci = {};
//	ci.cbSize = sizeof(CURSORINFO);
//
//	if (GetCursorInfo(&ci))
//	{
//		if (ci.flags == 0)
//			return false; // cursor is hidden
//		else if (ci.flags == 1)
//			return true; // cursor is visible
//		else if (ci.flags == 2)
//			return false; // cursor is suppressed
//		else
//			return false; // this shouldn't happen!
//	}
//	else
//	{
//		return false; // GetCursorInfo function failed
//	}
//}
//
////https://stackoverflow.com/questions/16110898/how-can-i-hide-the-mouse-cursor
//inline void showCursor()
//{
//	if (!cursorVisible())
//	{
//		while (ShowCursor(true) < 0);
//	}
//}
//
//inline void hideCursor()
//{
//	if (cursorVisible())
//	{
//		while (ShowCursor(false) >= 0);
//	}
//}