#include "stdafx.h"
#include "clientInput.h"
#include <filesystem/file/tcpSerializer.h>
#include "socketContainer.h"
#include <array/arrayfunctions.h>
#include "nbtSerializer.h"

bool clientInput::serialize(nbtSerializer &s)
{
	s.serializeValue(L"mouse position", mousePositionPixels);
	if (s.push(L"mouse buttons"))
	{
		s.serializeArray(L"clicked", (sbyte*)clicked, (int)sf::Mouse::ButtonCount);
		s.serializeArray(L"released", (sbyte*)clickReleased, (int)sf::Mouse::ButtonCount);
		s.serializeArray(L"holding", (sbyte*)holding, (int)sf::Mouse::ButtonCount);
		s.serializeValue(L"scrolldelta", scrollDelta);
		s.pop();
	}
	// s.serialize(keyDownHistory);
	if (s.push(L"keys"))
	{
		s.serializeList(L"down", keysDown);
		s.serializeList(L"up", keysUp);
		s.serializeList(L"holding", keysHolding);
		if (s.push(L"history"))
		{
			size_t size = keyDownHistory.size();
			s.serializeValue(L"size", size);
			if (!s.write)
			{
				keyDownHistory.resize(size);
			}
			for (size_t i = 0; i < size; i++)
			{
				if (s.push(std::to_wstring(i)))
				{
					typedef std::underlying_type_t<vk> ut;
					ut &k = (ut &)keyDownHistory[i].key;
					s.serializeValue(L"key", k);
					s.serializeValue(L"down", keyDownHistory[i].down);
					s.pop();
				}
			}

			s.pop();
		}
		s.pop();
	}
	s.serializeValue(L"text entered", textEntered);

	return true;
}

void clientInput::addClientInput(const clientInput &newInput)
{
	mousePositionPixels = newInput.mousePositionPixels;
	scrollDelta += newInput.scrollDelta;
	for (size_t i = 0; i < mb::ButtonCount; i++)
	{
		clicked[i] |= newInput.clicked[i];
		clickReleased[i] |= newInput.clickReleased[i];
		holding[i] = newInput.holding[i];
	}
	keyDownHistory.insert(keyDownHistory.end(), newInput.keyDownHistory.begin(), newInput.keyDownHistory.end());
	for (size_t i = 0; i < newInput.keysDown.size(); i++)
	{
		cvk &key = newInput.keysDown[i];
		if (std::find(keysDown.begin(), keysDown.end(), key) == keysDown.end())
		{
			keysDown.push_back(key);
		}
	}
	for (size_t i = 0; i < newInput.keysUp.size(); i++)
	{
		cvk &key = newInput.keysUp[i];
		if (std::find(keysUp.begin(), keysUp.end(), key) == keysUp.end())
		{
			keysUp.push_back(key);
		}
	}
	keysHolding = newInput.keysHolding;
	textEntered = newInput.textEntered;
}

bool clientInput::holdingDownKey(cvk &key) const
{
	return std::find(keysHolding.begin(), keysHolding.end(), key) != keysHolding.end();
}
bool clientInput::pressedKey(cvk &key) const
{
	return std::find(keysDown.begin(), keysDown.end(), key) != keysDown.end();
}

void clientInput::clearTemporaryData()
{
	fillAllElements(clicked, false);
	fillAllElements(clickReleased, false);
	keyDownHistory.clear();
	keysDown.clear();
	keysUp.clear();
	scrollDelta = 0;
	textEntered.clear();
}
