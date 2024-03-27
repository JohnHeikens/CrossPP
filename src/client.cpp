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

void client::render(cveci2& position, const texture& renderTarget)
{
	write = true;
	currentInput.serialize(*this);
	cbool& takeScreenShot = currentInput.pressedKey((vk)keyID::screenshot);
	auto screenSize = renderTarget.size;
	serialize(screenSize);
	s.sendPacket();
	currentInput.clearTemporaryData();

	write = false;

	sf::SocketSelector selector = sf::SocketSelector();
	selector.add(*s.socket);

	do {
		if (s.receivePacket() != sf::TcpSocket::Status::Done) {
			parent->switchVisibleChild(currentMainMenu);
			if (currentServer) {
				currentServer->stop();
				currentWorldSelector->refresh();
			}
			return;
		}
		//serialize colors of the screen
		//sounds
		serialize(earPosition);
		size_t soundCount;
		serialize(soundCount);
		for (size_t i = 0; i < soundCount; i++) {
			soundPacket packet = soundPacket();

			if (!packet.serialize(*this)) {
				break;
			}
			//globalSoundCollectionList[packet.soundCollectionID]->playSound()

			std::shared_ptr<sound2d> soundToPlay = std::make_shared<sound2d>(globalSoundCollectionList[packet.soundCollectionID]->audioToChooseFrom[packet.soundIndex].get(), packet.position, packet.volume, packet.pitch, true);
			handler->playAudio(soundToPlay);
		}
		//screen

		//std::vector<colorChannel> channels[rgbColorChannelCount];
		////unpack
		//for (int i = 0; i < rgbColorChannelCount; i++) {
		//	serialize(channels[i]);
		//}
		//color* ptr = renderTarget.baseArray;
		//for (fsize_t i = 0; i < renderTarget.size.volume(); i++, ptr++) {
		//	*ptr = color(channels[2][i], channels[1][i], channels[0][i]);
		//}

		//if (serialize((color::channelType*)renderTarget.baseArray, renderTarget.size.volume() * bgraColorChannelCount)) {
		//
		//}
	} while (selector.wait(sf::microseconds(1)));//pop off all packets on the chain and catch up
	std::vector<byte> compressedScreen;
	if (serialize(compressedScreen)) {
		std::vector<byte> decompressedScreen;
		vectn<fsize_t, 2> size;
		uint32_t channelCount;
		fpng::fpng_decode_memory((const char*)&(*compressedScreen.begin()), (uint32_t)compressedScreen.size(), decompressedScreen, size.x, size.y, channelCount, bgraColorChannelCount);
		if (size == renderTarget.size) {
			std::copy(decompressedScreen.begin(), decompressedScreen.end(), (byte*)renderTarget.baseArray);
		}
		else {
			texture oldSizeTex = texture(size, false);
			std::copy(decompressedScreen.begin(), decompressedScreen.end(), (byte*)oldSizeTex.baseArray);
			fillTransformedTexture(crectangle2(renderTarget.getClientRect()), oldSizeTex, renderTarget);
		}
	}
	if (takeScreenShot) {
		//take screenshot

		createFoldersIfNotExists(screenshotsFolder);
		renderTarget.Flip();
		for (color& c : renderTarget) {
			c.a() = color::maxValue;
		}

		renderTarget.Save(screenshotsFolder + timeToString(L"%Y-%m-%d_%H.%M.%S") + L".png");
		lastScreenshotTime = getMiliseconds();
	}
	//0.5 seconds flash
	if (getMiliseconds() - lastScreenshotTime < 500) {
		//renderTarget.Save(screenshotsFolder + timeToString(L"%Y-%m-%d_%H.%M.%S") + L".bmp");//for testing
		renderTarget.fill(colorPalette::white);//flash effect
	}
}

client::client()
{
	if (!data.serialize(false)) {
		data.id = randomUUID(currentRandom);

		//while (true) {
		//	output(randomName(currentRandom) + L"\n");
		//}

		data.name = randomName(currentRandom);
		data.serialize(true);
	}
}
void client::mouseDown(cveci2& position, cmb& button)
{
	currentInput.clicked[button] = true;
	currentInput.holding[button] = true;
}
void client::mouseUp(cveci2& position, cmb& button)
{
	currentInput.clickReleased[button] = true;
	currentInput.holding[button] = false;
}
void client::scroll(cveci2& position, cint& scrollDelta)
{
	currentInput.scrollDelta += scrollDelta;
}
void client::enterText(cuint& keyCode)
{
	currentInput.textEntered += keyCode;
}
void client::hover(cveci2& position)
{
	currentInput.mousePositionPixels = position;
}

void client::keyDown(cvk& keyCode)
{
	addWithoutDupes(currentInput.keysDown, keyCode);
	currentInput.keysHolding.push_back(keyCode);
	currentInput.keyDownHistory.push_back(keyHistoryEvent(keyCode, true));
}

void client::keyUp(cvk& keyCode)
{
	currentInput.keysUp.push_back(keyCode);
	const auto& it = std::find(currentInput.keysHolding.begin(), currentInput.keysHolding.end(), keyCode);
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

bool client::connectToServer(const serverData& server)
{
	s.socket = new sf::TcpSocket();
	sf::Socket::Status status = s.socket->connect(server.serverIPAddress, server.serverPort, sf::seconds(5));
	if (status != sf::Socket::Done)
	{
		if (status == sf::Socket::Error) {
			return false;
		}
		// error...
	}

	write = true;
	//ull num = gameAuthNumber;
	//unsigned long long num = gameAuthNumber;
	//construct authentication packet
	ull num = gameAuthNumber;
	serialize(num);
	serialize(this->rect.size);
	serialize(data.id);
	serializeWStringAsString(data.name);
	s.sendPacket();
	return true;
	//char data[100]{};
	//
	//// TCP socket:
	//if (socket.send(data, 100) != sf::Socket::Done)
	//{
	//	// error...
	//}

}
