#include "TextBox.h"
#include "math/timemath.h"
#include "application/interaction.h"

textBox::textBox() :control()
{
}

void textBox::render(cveci2& position, const texture& renderTarget)
{
	control::render(position, renderTarget);

	//draw cursor
	if (focused && GetSecond() % 2)
	{
		checkCursorIndex();
		rectangle2 textRect = rectangle2(position, rect.size).expanded(-borderSize);
		vec2 cursorPosition = currentFont->MeasureStringOffset(textRect, text.substr(0, cursorIndex));
		renderTarget.fillRectangle(crectangle2(cursorPosition + vec2(0, -currentFont->fontSize), vec2(1, currentFont->fontSize)), brushes::white);
	}
}

void textBox::keyDown(cvk& keyCode)
{
	checkCursorIndex();
	//cbool shift = pressed(VK_LSHIFT) || pressed(VK_RSHIFT);
	shift |= (keyCode == VK_SHIFT);
	capsLock |= (keyCode == VK_CAPITAL);
	cbool upperCase = shift ^ capsLock;
	//MapVirtualKey(keyCode, MAPVK_VK_TO_CHAR)
	if (keyCode != VK_SHIFT)
	{
		if (keyCode == VK_BACK)
		{
			if (cursorIndex > 0)
			{
				text = text.erase(cursorIndex - 1, 1);
				cursorIndex--;
			}
		}
		else if (keyCode == VK_DELETE)
		{
			if (text.length() > cursorIndex)
			{
				text = text.erase(cursorIndex, 1);
			}
		}
		else if (keyCode == VK_LEFT)
		{
			if (cursorIndex > 0)
			{
				cursorIndex--;
			}
		}
		else if (keyCode == VK_RIGHT)
		{
			if (cursorIndex < text.length())
			{
				cursorIndex++;
			}
		}
		else if ((keyCode == VK_LWIN) || (keyCode == VK_RWIN))
		{
			//do nothing
		}
		else
		{
			letter resultingCharachter;
			if (!getLetter(keyCode, upperCase, resultingCharachter))
			{
				switch (keyCode)
				{
				case VK_OEM_1:
				{
					resultingCharachter = shift ? L':' : L';';
				}
				break;
				case VK_OEM_2:
				{
					resultingCharachter = L'/';
				}
				break;
				case VK_OEM_3:
				{
					resultingCharachter = shift ? L'~' : L'`';
				}
				break;
				case VK_OEM_4:
				{
					resultingCharachter = shift ? L'{' : L'[';
				}
				break;
				case VK_OEM_5:
				{
					resultingCharachter = shift ? L'|' : L'\\';
				}
				break;
				case VK_OEM_6:
				{
					resultingCharachter = shift ? L'}' : L']';
				}
				break;
				case VK_OEM_7:
				{
					resultingCharachter = L'\"';
				}
				break;
				case VK_OEM_COMMA:
				{
					resultingCharachter = shift ? L'<' : L',';
				}
				break;
				case VK_OEM_PERIOD:
				{
					resultingCharachter = shift ? L'>' : L'.';
				}
				break;
				case VK_OEM_MINUS:
				{
					resultingCharachter = shift ? L'_' : L'-';
				}
				break;
				default:
					//letter not recognised
					resultingCharachter = keyCode;
				}
			}
			const std::wstring& doubleCharachters = std::wstring(L"()[]{}<>\"\"");
			const std::wstring& alphabet = std::wstring(L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
			const std::wstring allowedCharachters = doubleCharachters + alphabet + std::wstring(L"0123456789!@#$%^&.,:;-+*_~`|\\/\n\t ");
			if (allowedCharachters.find(resultingCharachter) != std::wstring::npos)
			{
				text.insert(cursorIndex, 1, resultingCharachter);
				cursorIndex++;
			}

			csize_t& doubleCharachterIndex = doubleCharachters.find(resultingCharachter);
			if ((doubleCharachterIndex != std::wstring::npos) && ((doubleCharachterIndex % 2) == 0))
			{
				text.insert(cursorIndex, 1, doubleCharachters[doubleCharachterIndex + 1]);
			}
		}
	}
}

void textBox::keyUp(cvk& keyCode)
{
	shift &= (keyCode != VK_SHIFT);
	capsLock &= (keyCode != VK_CAPITAL);
}

void textBox::lostFocus()
{
	shift = false;
	capsLock = false;
}

void textBox::paste()
{
	checkCursorIndex();
	std::wstring clipBoardText;
	if (GetClipboardText(clipBoardText))
	{
		text.insert(cursorIndex, clipBoardText);
		cursorIndex += clipBoardText.length();
	}
}

void textBox::checkCursorIndex()
{
	if (cursorIndex > text.length())
	{
		cursorIndex = text.length();
	}
}
