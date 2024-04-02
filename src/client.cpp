#include "client.h"
#include <array/arrayfunctions.h>
#include "soundPacket.h"
#include "soundCollection.h"
#include <memory>
#include "mainMenu.h"
#include "server.h"
#include "worldSelector.h"
#include "keyHistoryEvent.h"
#include "fpng.h"
#include <cstdint>
#include <vector>
#include <SFML/Network/Socket.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/System/Time.hpp>
#include "constants.h"
#include "application/mouseButton.h"
#include "GlobalFunctions.h"
#include "math/graphics/color/color.h"
#include "math/graphics/texture.h"
#include "math/random/random.h"
#include "math/vectn.h"
#include "serverData.h"
#include "soundhandler2d.h"
#include "folderList.h"
#include "math/timemath.h"
#include <filesystem/filemanager.h>

miliseconds lastScreenshotTime = 0;

void client::render(cveci2 &position, const texture &renderTarget)
{
	write = true;

	
	cbool &takeScreenShot = currentInput.pressedKey((vk)keyID::screenshot);

	sendPacket(renderTarget);

	processIncomingPackets(renderTarget);
	if (takeScreenShot)
	{
		// take screenshot

		createFoldersIfNotExists(screenshotsFolder);
		renderTarget.Flip();
		for (color &c : renderTarget)
		{
			c.a() = color::maxValue;
		}

		renderTarget.Save(screenshotsFolder / (timeToString(L"%Y-%m-%d_%H.%M.%S") + L".png"));
		lastScreenshotTime = getMiliseconds();
	}
	// 0.5 seconds flash
	if (getMiliseconds() - lastScreenshotTime < 500)
	{
		// renderTarget.Save(screenshotsFolder / timeToString(L"%Y-%m-%d_%H.%M.%S") + L".bmp");//for testing
		renderTarget.fill(colorPalette::white); // flash effect
	}
}

client::client()
{
	if (!data.serialize(false))
	{
		data.id = randomUUID(currentRandom);

		// while (true) {
		//	output(randomName(currentRandom) + L"\n");
		// }

		data.name = randomName(currentRandom);
		data.serialize(true);
	}
}
void client::mouseDown(cveci2 &position, cmb &button)
{
	currentInput.clicked[button] = true;
	currentInput.holding[button] = true;
}
void client::mouseUp(cveci2 &position, cmb &button)
{
	currentInput.clickReleased[button] = true;
	currentInput.holding[button] = false;
}
void client::scroll(cveci2 &position, cint &scrollDelta)
{
	currentInput.scrollDelta += scrollDelta;
}
void client::enterText(cuint &keyCode)
{
	currentInput.textEntered += keyCode;
}
void client::hover(cveci2 &position)
{
	currentInput.mousePositionPixels = position;
}

void client::keyDown(cvk &keyCode)
{
	addWithoutDupes(currentInput.keysDown, keyCode);
	currentInput.keysHolding.push_back(keyCode);
	currentInput.keyDownHistory.push_back(keyHistoryEvent(keyCode, true));
}

void client::keyUp(cvk &keyCode)
{
	currentInput.keysUp.push_back(keyCode);
	const auto &it = std::find(currentInput.keysHolding.begin(), currentInput.keysHolding.end(), keyCode);
	if (it != currentInput.keysHolding.end())
	{
		currentInput.keysHolding.erase(it);
	}
	currentInput.keyDownHistory.push_back(keyHistoryEvent(keyCode, false));
}

void client::lostFocus()
{
	currentInput.keysHolding.clear();
}

bool client::connectToServer(const serverData &server)
{
	s.socket = new sf::TcpSocket();
	sf::Socket::Status status = s.socket->connect(server.serverIPAddress, server.serverPort, sf::seconds(5));
	if (status != sf::Socket::Done)
	{
		if (status == sf::Socket::Error)
		{
			return false;
		}
		// error...
	}

	write = true;
	// ull num = gameAuthNumber;
	// unsigned long long num = gameAuthNumber;
	// construct authentication packet
	ull num = gameAuthNumber;
	serialize(num); // this is to distinguish between random things pinging this port and clients trying to connect
	nbtCompound authPacket = nbtCompound(L"auth");
	nbtSerializer outNBTSerializer = nbtSerializer(authPacket, true);
	outNBTSerializer.serializeValue(L"uuid", data.id);
	outNBTSerializer.serializeValue(L"name", data.name);
	outNBTSerializer.serializeValue(L"screenSize", this->rect.size);
	streamSerializer streamS = streamSerializer(s, true, std::endian::big);
	authPacket.serialize(streamS);
	s.sendPacket();
	selector.add(*s.socket);
	return true;
	// char data[100]{};
	//
	//// TCP socket:
	// if (socket.send(data, 100) != sf::Socket::Done)
	//{
	//	// error...
	// }
}

void client::sendPacket(const texture &renderTarget)
{
	nbtCompound outCompound = nbtCompound(std::wstring(L"packetOut"));
	nbtSerializer outSerializer = nbtSerializer(outCompound, true);

	currentInput.serialize(outSerializer);
	auto screenSize = renderTarget.size;
	outSerializer.serializeValue(L"screenSize", screenSize);

	streamSerializer streamS = streamSerializer(s, true, std::endian::big);
	outCompound.serialize(streamS);
	s.sendPacket();
	currentInput.clearTemporaryData();
}

void client::processIncomingPackets(const texture &renderTarget)
{
		write = false;

	nbtSerializer *inSerializer = nullptr;
	nbtCompound* inCompound = nullptr;

	// receive packets, but don't process the screen if another packet is waiting, so we can catch up
	do
	{
		delete inCompound;
		inCompound = new nbtCompound(std::wstring(L"packetIn"));
		if (s.receivePacket() != sf::TcpSocket::Status::Done)
		{
			//disconnect, in case of any error
			s.socket->disconnect();
			selector.remove(*s.socket);
			parent->switchVisibleChild(currentMainMenu);
			if (currentServer)
			{
				currentServer->stop();
				currentWorldSelector->refresh();
			}
			return;
		}
		else
		{
			streamSerializer streamS = streamSerializer(s, false, std::endian::big);
			inCompound->serialize(streamS);
		}
		delete inSerializer;
		inSerializer = new nbtSerializer(*inCompound, false);
		// serialize colors of the screen
		// sounds
		inSerializer->serializeValue(L"earPosition", earPosition);
		size_t soundCount;
		if (inSerializer->push<nbtDataTag::tagList>(L"sounds"))
		{
			const std::vector<nbtData *> &serializedSoundList = inSerializer->getChildren();
			for (const nbtData *serializedSoundData : serializedSoundList)
			{
				if (inSerializer->push(serializedSoundData))
				{

					soundPacket packet = soundPacket();

					if (!packet.serialize(*inSerializer))
					{
						inSerializer->pop();
						break;
					}
					// globalSoundCollectionList[packet.soundCollectionID]->playSound()

					std::shared_ptr<sound2d> soundToPlay = std::make_shared<sound2d>(globalSoundCollectionList[packet.soundCollectionID]->audioToChooseFrom[packet.soundIndex].get(), packet.position, packet.volume, packet.pitch, true);
					handler->playAudio(soundToPlay);
					inSerializer->pop();
				}
			}
			inSerializer->pop();
		}
	} while (selector.wait(sf::microseconds(1))); // pop off all packets on the chain and catch up

	std::vector<byte> compressedScreen;
	if (inSerializer->serializeList(L"compressedScreen", compressedScreen))
	{
		std::vector<byte> decompressedScreen;
		vectn<fsize_t, 2> size;
		uint32_t channelCount;
		fpng::fpng_decode_memory((const char *)&(*compressedScreen.begin()), (uint32_t)compressedScreen.size(), decompressedScreen, size.x, size.y, channelCount, bgraColorChannelCount);
		if (size == renderTarget.size)
		{
			std::copy(decompressedScreen.begin(), decompressedScreen.end(), (byte *)renderTarget.baseArray);
		}
		else
		{
			texture oldSizeTex = texture(size, false);
			std::copy(decompressedScreen.begin(), decompressedScreen.end(), (byte *)oldSizeTex.baseArray);
			fillTransformedTexture(crectangle2(renderTarget.getClientRect()), oldSizeTex, renderTarget);
		}
	}
	delete inSerializer;
	delete inCompound;
}
