#pragma once
#include <SFML/Network.hpp>
#include "entity.h"
#include "gameControl.h"
#include "filesystem/file/endian.h"
#include "socketContainer.h"
struct playerSocket : socketContainer {
	human* player;
	gameControl* screen = nullptr;
	bool authenticated = false;
	playerSocket(sf::TcpSocket* socket);
	texture* lastRenderResult = nullptr;
	void processSocketInput();
	~playerSocket();
	bool shouldDisconnect = false;
	std::thread* sendRenderResultThread = nullptr;
};
void renderAsync(playerSocket* socket);
void sendRenderResultAsync(playerSocket* socket);