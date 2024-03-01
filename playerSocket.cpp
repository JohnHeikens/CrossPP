#include "playerSocket.h"
#include "world.h"
#include <string>
#include <SFML/Network/Socket.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include "constants.h"
#include "gameControl.h"
#include "array/arrayfunctions.h"
#include "GlobalFunctions.h"
#include "math/graphics/color/color.h"
#include "math/graphics/texture.h"
#include "math/rectangletn.h"
#include "math/uuid.h"
#include "math/vectn.h"
#include "human.h"

playerSocket::playerSocket(sf::TcpSocket* socket)
{
	screen = new gameControl(*this);
	this->s.socket = socket;
	write = false;

	ull num;
	//receive authentication packet
	s.receivePacket();
	serialize(num);
	if (num != gameAuthNumber)
	{
		return;
	}
	authenticated = true;

	rectanglei2 screenRect = rectanglei2();
	serialize(screenRect.size);
	screen->layout(screenRect);

	uuid playerUUID;
	serialize(playerUUID);
	std::wstring playerName;
	serializeWStringAsString(playerName);
	player = currentWorld->joinPlayer(playerUUID, playerName, *screen);
	screen->player = player;

	player->serialize(false);
}

void playerSocket::processSocketInput()
{

}

playerSocket::~playerSocket()
{
	s.socket->disconnect();
	delete s.socket;
}

void renderAsync(playerSocket* socket)
{
	if (socket->lastRenderResult) {
		socket->sendRenderResultThread->join();//wait until the last image has finished sending
		delete socket->lastRenderResult;//then delete the last render result
		delete socket->sendRenderResultThread;
	}
	socket->lastRenderResult = new texture(socket->screen->rect.size);
	socket->screen->render(cveci2(), *socket->lastRenderResult);
	socket->screen->player->updateBodyParts();//update body parts so the correct ear position can be obtained
	socket->write = true;

	//don't serialize this part async, as it doesn't take much time and might cause corruption when for example sounds are added mid-sending
	//send sounds

	vec2 earPosition = socket->screen->player->getHeadPosition();

	socket->serialize(earPosition);

	size_t soundCount = socket->screen->dataToSend.size();
	socket->serialize(soundCount);
	for (auto& data : socket->screen->dataToSend) {
		data.serialize(*socket);
	}
	socket->screen->dataToSend.clear();

	//TODO: serialize the screen in 'sendrenderresultasync'
	socket->serialize((color::channelType*)socket->lastRenderResult->baseArray, socket->screen->rect.size.volume() * bgraColorChannelCount);

	socket->sendRenderResultThread = new std::thread(sendRenderResultAsync, socket);

	//finally, send the packet with all the data of this render cycle to the player
	if (is_in(socket->s.receivePacket(), sf::Socket::Status::Disconnected, sf::Socket::Status::Error)) {//client disconnected?
		socket->shouldDisconnect = true;
	}
	else {
		socket->write = false;
		//read input while sending
		socket->screen->mostRecentInput.serialize(*socket);
		//socket->s.socket->setBlocking(true);
		socket->screen->addClientInput(socket->screen->mostRecentInput);
		socket->screen->processInput();
	}
}

void sendRenderResultAsync(playerSocket* socket)
{
	//serialize screen and finally, send the packet

	socket->s.sendPacket();
}
