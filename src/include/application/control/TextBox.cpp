#include "TextBox.h"
#include "math/timemath.h"
#include "application/interaction.h"

textBox::textBox() : control()
{
}

void textBox::render(cveci2& position, const texture& renderTarget)
{
	control::render(position, renderTarget);

	// draw cursor
	if (focused && GetSecond() % 2)
	{
		checkCursorIndex();
		rectangle2 textRect = rectangle2(position, rect.size).expanded(-borderSize);
		vec2 cursorPosition = currentFont->MeasureStringOffset(textRect, text.substr(0, cursorIndex));
		renderTarget.fillRectangle(crectangle2(cursorPosition, vec2(1, currentFont->fontSize)), brushes::white);
	}
}

void textBox::enterText(cuint& uniCode)
{
	checkCursorIndex();
	if (uniCode == L'\b') {//backspace
		if (cursorIndex) {
			text = text.erase(cursorIndex - 1, 1);
			cursorIndex--;
		}
	}
	else {
		text.insert(cursorIndex, 1, (wchar_t)uniCode);
		cursorIndex++;
	}

	// cbool shift = pressed(VK_LSHIFT) || pressed(VK_RSHIFT);
	// shift |= (keyCode == vk::LShift);
	////capsLock |= (keyCode == vk::);
	// cbool upperCase = shift ^ capsLock;
	////MapVirtualKey(keyCode, MAPVK_VK_TO_CHAR)
	// if (keyCode != vk::LShift)
	//{
	//	if (keyCode == vk::Backspace)
	//	{
	//		if (cursorIndex > 0)
	//		{
	//			text = text.erase(cursorIndex - 1, 1);
	//			cursorIndex--;
	//		}
	//	}
	//	else if (keyCode == vk::Delete)
	//	{
	//		if (text.length() > cursorIndex)
	//		{
	//			text = text.erase(cursorIndex, 1);
	//		}
	//	}
	//	else if (keyCode == vk::Left)
	//	{
	//		if (cursorIndex > 0)
	//		{
	//			cursorIndex--;
	//		}
	//	}
	//	else if (keyCode == vk::Right)
	//	{
	//		if (cursorIndex < text.length())
	//		{
	//			cursorIndex++;
	//		}
	//	}
	//	//else if ((keyCode == vk::wi) || (keyCode == VK_RWIN))
	//	//{
	//	//	//do nothing
	//	//}
	//	else
	//	{
	//		letter resultingCharachter;
	//		if (!getLetter(keyCode, upperCase, resultingCharachter))
	//		{
	//			switch (keyCode)
	//			{
	//			case vk::SemiColon:
	//			{
	//				resultingCharachter = shift ? L':' : L';';
	//			}
	//			break;
	//			case VK_OEM_2:
	//			{
	//				resultingCharachter = L'/';
	//			}
	//			break;
	//			case VK_OEM_3:
	//			{
	//				resultingCharachter = shift ? L'~' : L'`';
	//			}
	//			break;
	//			case VK_OEM_4:
	//			{
	//				resultingCharachter = shift ? L'{' : L'[';
	//			}
	//			break;
	//			case VK_OEM_5:
	//			{
	//				resultingCharachter = shift ? L'|' : L'\\';
	//			}
	//			break;
	//			case VK_OEM_6:
	//			{
	//				resultingCharachter = shift ? L'}' : L']';
	//			}
	//			break;
	//			case VK_OEM_7:
	//			{
	//				resultingCharachter = L'\"';
	//			}
	//			break;
	//			case VK_OEM_COMMA:
	//			{
	//				resultingCharachter = shift ? L'<' : L',';
	//			}
	//			break;
	//			case VK_OEM_PERIOD:
	//			{
	//				resultingCharachter = shift ? L'>' : L'.';
	//			}
	//			break;
	//			case VK_OEM_MINUS:
	//			{
	//				resultingCharachter = shift ? L'_' : L'-';
	//			}
	//			break;
	//			default:
	//				//letter not recognised
	//				resultingCharachter = keyCode;
	//			}
	//		}
	//		const std::wstring& doubleCharachters = std::wstring(L"()[]{}<>\"\"");
	//		const std::wstring& alphabet = std::wstring(L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
	//		const std::wstring allowedCharachters = doubleCharachters + alphabet + std::wstring(L"0123456789!@#$%^&.,:;-+*_~`|\\/\n\t ");
	//		if (allowedCharachters.find(resultingCharachter) != std::wstring::npos)
	//		{
	//			text.insert(cursorIndex, 1, resultingCharachter);
	//			cursorIndex++;
	//		}
	//
	//		csize_t& doubleCharachterIndex = doubleCharachters.find(resultingCharachter);
	//		if ((doubleCharachterIndex != std::wstring::npos) && ((doubleCharachterIndex % 2) == 0))
	//		{
	//			text.insert(cursorIndex, 1, doubleCharachters[doubleCharachterIndex + 1]);
	//		}
	//	}
	//}
}

void textBox::keyDown(cvk& keyCode)
{
	checkCursorIndex();
	if (keyCode == vk::Delete && cursorIndex < text.length())
	{
		text = text.erase(cursorIndex, 1);
	}
	else if (keyCode == vk::Left && cursorIndex > 0)
	{
		cursorIndex--;
	}
	else if (keyCode == vk::Right && cursorIndex < text.length())
	{
		cursorIndex++;
	}
}

//void textBox::keyUp(cvk &keyCode)
//{
//	shift &= (keyCode != VK_SHIFT);
//	capsLock &= (keyCode != VK_CAPITAL);
//}

void textBox::lostFocus()
{
	shift = false;
	capsLock = false;
}

void textBox::paste()
{
	checkCursorIndex();
	//std::wstring clipBoardText;
	//if (GetClipboardText(clipBoardText))
	//{
	//	text.insert(cursorIndex, clipBoardText);
	//	cursorIndex += clipBoardText.length();
	//}
}

void textBox::mouseDown(cveci2& position, cmb& button)
{
	// check position of cursor

	crectangle2& relativeTextRect = crectangle2(rectanglei2(rect.size).expanded(-borderSize));
	vec2 offset = currentFont->MeasureStringOffset(relativeTextRect, L"");
	fp closestDistance = INFINITY;
	fsize_t closestIndex = 0;
	for (fsize_t i = 0;; i++, offset = currentFont->MeasureLetterOffset(relativeTextRect, offset, text[i]))
	{
		if (position.y > offset.y && position.y < offset.y + currentFont->fontSize)
		{
			cfp& distance = math::absolute(position.x - offset.x);
			if (distance < closestDistance)
			{
				closestDistance = distance;
				closestIndex = i;
			}
		}
		if (i == text.length())
		{
			break;
		}
	}
	if (closestDistance != INFINITY)
	{
		cursorIndex = closestIndex;
	}
	else
	{
		if (position.y < offset.y + currentFont->fontSize)
		{
			cursorIndex = text.length();
		}
		else
		{
			cursorIndex = 0;
		}
	}
}

void textBox::checkCursorIndex()
{
	if (cursorIndex > text.length())
	{
		cursorIndex = text.length();
	}
}
