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
#include "server.h"
#include <future>

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
	texture* newRenderResult = new texture(socket->screen->rect.size);
	socket->screen->render(cveci2(), *newRenderResult);
	socket->screen->player->updateBodyParts();//update body parts so the correct ear position can be obtained

	if (socket->lastRenderResult) {
		socket->sendRenderResultThread->join();//wait until the last image has finished sending
		delete socket->lastRenderResult;//then delete the last render result
		delete socket->sendRenderResultThread;
	}
	socket->lastRenderResult = newRenderResult;
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


	socket->sendRenderResultThread = new std::thread(sendRenderResultAsync, socket);

	sf::SocketSelector selector;
	selector.add(*socket->s.socket);
	selector.wait(sf::microseconds(1));
	//we don't wait for input, we just process the input in the next frame. too bad, it would cause server lag
	while (selector.isReady(*socket->s.socket)) {//the wait() of the socket is called in the main server loop once a tick
		//finally, send the packet with all the data of this render cycle to the player
		if (is_in(socket->s.receivePacket(), sf::Socket::Status::Disconnected, sf::Socket::Status::Error)) {//client disconnected?
			socket->shouldDisconnect = true;
			return;
		}
		else {
			//selector should continue looking if the socket is still ready
			socket->write = false;
			
			//read input while sending
			socket->screen->mostRecentInput.serialize(*socket);
			//socket->s.socket->setBlocking(true);
			socket->screen->addClientInput(socket->screen->mostRecentInput);
			socket->screen->processInput();
			selector.wait(sf::microseconds(1));
		}
	}
}

void sendRenderResultAsync(playerSocket* socket)
{
	//serialize screen and finally, send the packet

	//TODO: serialize the screen in 'sendrenderresultasync'. this is hard because s->write = false
	//we don't use the normal 'serialize' function so we don't have to use the 'write' boolean
	socket->s.write((char*)(color::channelType*)socket->lastRenderResult->baseArray, socket->screen->rect.size.volume() * bgraColorChannelCount * sizeof(color::channelType));

	socket->s.sendPacket();
}
