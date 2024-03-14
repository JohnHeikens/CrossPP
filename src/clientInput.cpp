#include "stdafx.h"
#include "clientInput.h"
#include <filesystem/file/tcpSerializer.h>
#include "socketContainer.h"
#include <array/arrayfunctions.h>

bool clientInput::serialize(const socketContainer& s)
{
	s.serialize(mousePositionPixels);
	s.serialize(clicked);
	s.serialize(clickReleased);
	s.serialize(holding);
	//s.serialize(keyDownHistory);
	s.serialize(keysDown);
	s.serialize(keysUp);
	s.serialize(keysHolding);
	s.serialize(scrollDelta);

	size_t size = keyDownHistory.size();
	s.serialize(size);
	if (!s.write) {
		keyDownHistory.resize(size);
	}
	for (size_t i = 0; i < size; i++)
	{
		s.serialize(keyDownHistory[i].key);
		s.serialize(keyDownHistory[i].down);
	}
	
	return true;
}

void clientInput::addClientInput(const clientInput& newInput)
{
	mousePositionPixels = newInput.mousePositionPixels;
	scrollDelta += newInput.scrollDelta;
	for (int i = 0; i < mouseButton::count; i++) {
		clicked[i] |= newInput.clicked[i];
		clickReleased[i] |= newInput.clickReleased[i];
		holding[i] = newInput.holding[i];
	}
	keyDownHistory.insert(keyDownHistory.end(), newInput.keyDownHistory.begin(), newInput.keyDownHistory.end());
	for (int i = 0; i < newInput.keysDown.size(); i++) {
		cvk& key = newInput.keysDown[i];
		if (std::find(keysDown.begin(), keysDown.end(), key) == keysDown.end()) {
			keysDown.push_back(key);
		}
	}
	for (int i = 0; i < newInput.keysUp.size(); i++) {
		cvk& key = newInput.keysUp[i];
		if (std::find(keysUp.begin(), keysUp.end(), key) == keysUp.end()) {
			keysUp.push_back(key);
		}
	}
	keysHolding = newInput.keysHolding;
}

bool clientInput::holdingDownKey(cvk& key) const
{
	return std::find(keysHolding.begin(), keysHolding.end(), key) != keysHolding.end();
}
bool clientInput::pressedKey(cvk& key) const
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

}
