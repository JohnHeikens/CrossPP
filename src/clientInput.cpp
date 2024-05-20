#include "stdafx.h"
#include "clientInput.h"
#include <filesystem/file/tcpSerializer.h>
#include "socketContainer.h"
#include "array/arrayFunctions/arrayFunctions.h"
#include "nbtSerializer.h"
#include "serializer/serializeColor.h"
#include "serializer/serializeList.h"

bool clientInput::serialize(nbtSerializer &s)
{
    if(s.push(L"events")){
        size_t size = eventHistory.size();
        s.serializeValue(L"size", size);
        if (!s.write)
        {
            eventHistory.resize(size);
        }
        for (size_t i = 0; i < size; i++)
        {
            if (s.push(std::to_wstring(i)))
            {
                //typedef std::underlying_type_t<sf::Event::EventType> ut;
                //ut &k = (ut &)eventHistory[i].type;
                s.serializeValue(L"type", eventHistory[i].type);
                switch (eventHistory[i].type) {
                    case sf::Event::TouchMoved:
                    case sf::Event::TouchBegan:
                    case sf::Event::TouchEnded:
                        s.serializeValue(L"x", eventHistory[i].touch.x);
                        s.serializeValue(L"y", eventHistory[i].touch.y);
                        s.serializeValue(L"finger", eventHistory[i].touch.finger);

                        break;
                    case sf::Event::MouseButtonPressed:
                    case sf::Event::MouseButtonReleased:
                        s.serializeValue(L"x", eventHistory[i].mouseButton.x);
                        s.serializeValue(L"y", eventHistory[i].mouseButton.y);
                        s.serializeValue(L"button", eventHistory[i].mouseButton.button);
                        break;
                    case sf::Event::MouseMoved:
                        s.serializeValue(L"x", eventHistory[i].mouseMove.x);
                        s.serializeValue(L"y", eventHistory[i].mouseMove.y);
                        break;
                    case sf::Event::KeyPressed:
                    case sf::Event::KeyReleased:
                        //we don't need to serialize the scan code, that is the local key code.
                        //when reading this data on another device, the result would be completely different.
                        s.serializeValue(L"alt", eventHistory[i].key.alt);
                        s.serializeValue(L"control", eventHistory[i].key.control);
                        s.serializeValue(L"shift", eventHistory[i].key.shift);
                        s.serializeValue(L"code", eventHistory[i].key.code);
                        break;
                    case sf::Event::SensorChanged:
                        s.serializeValue(L"x", eventHistory[i].sensor.x);
                        s.serializeValue(L"y", eventHistory[i].sensor.y);
                        s.serializeValue(L"z", eventHistory[i].sensor.z);
                        break;
                    case sf::Event::MouseWheelScrolled:
                        s.serializeValue(L"x", eventHistory[i].mouseWheelScroll.x);
                        s.serializeValue(L"y", eventHistory[i].mouseWheelScroll.y);
                        s.serializeValue(L"delta", eventHistory[i].mouseWheelScroll.delta);
                        s.serializeValue(L"wheel", eventHistory[i].mouseWheelScroll.wheel);
                        break;
                    case sf::Event::TextEntered:
                        s.serializeValue(L"unicode", eventHistory[i].text.unicode);
                        break;
                    case sf::Event::Resized:
                        s.serializeValue(L"width", eventHistory[i].size.width);
                        s.serializeValue(L"height", eventHistory[i].size.height);
                        break;
                    case sf::Event::MouseEntered://no data associated with these
                    case sf::Event::MouseLeft:
                    case sf::Event::Closed:
                    case sf::Event::LostFocus:
                    case sf::Event::GainedFocus:
                    default:
                    break;
                }
                s.pop();
            }
        }
        s.pop();
    }
	serializeNBTValue(s, L"mouse position", mousePositionPixels);
	if (s.push(L"mouse buttons"))
	{
		s.serializeArray(L"clicked", (sbyte*)clicked, (size_t)sf::Mouse::ButtonCount);
		s.serializeArray(L"released", (sbyte*)clickReleased, (size_t)sf::Mouse::ButtonCount);
		s.serializeArray(L"holding", (sbyte*)holding, (size_t)sf::Mouse::ButtonCount);
		s.serializeValue(L"scrolldelta", scrollDelta);
		s.pop();
	}
	// s.serialize(keyDownHistory);
	if (s.push(L"keys"))
	{
		serializeNBTValue(s, L"down", keysDown);
		serializeNBTValue(s, L"up", keysUp);
		serializeNBTValue(s, L"holding", keysHolding);
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
    eventHistory.clear();
	keysDown.clear();
	keysUp.clear();
	scrollDelta = 0;
	textEntered.clear();
}
