#include "client.h"
#include <array/arrayfunctions.h>
#include "soundPacket.h"
#include "soundCollection.h"
#include <memory>
#include "math/uuid.h"
#include "folderList.h"
#include "nbtSerializer.h"
#include "mainMenu.h"
#include "server.h"
#include "worldSelector.h"
#include "keyHistoryEvent.h"

void client::render(cveci2& position, const texture& renderTarget)
{
	write = true;
	currentInput.serialize(*this);
	s.sendPacket();
	currentInput.clearTemporaryData();

	write = false;

	if (s.receivePacket() == sf::TcpSocket::Status::Disconnected) {
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
		packet.serialize(*this);
		//globalSoundCollectionList[packet.soundCollectionID]->playSound()

		std::shared_ptr<sound2d> soundToPlay = std::make_shared<sound2d>(globalSoundCollectionList[packet.soundCollectionID]->audioToChooseFrom[packet.soundIndex].get(), packet.position, packet.volume, packet.pitch, true);
		handler->playAudio(soundToPlay);
	}
	//screen
	if (serialize((color::channelType*)renderTarget.baseArray, renderTarget.size.volume() * bgraColorChannelCount)) {

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
void client::mouseDown(cveci2& position, cvk& button)
{
	currentInput.clicked[keyCodeToButton(button)] = true;
	currentInput.holding[keyCodeToButton(button)] = true;
}
void client::mouseUp(cveci2& position, cvk& button)
{
	currentInput.clickReleased[keyCodeToButton(button)] = true;
	currentInput.holding[keyCodeToButton(button)] = false;
}
void client::scroll(cveci2& position, cint& scrollDelta)
{
	currentInput.scrollDelta += scrollDelta;
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
