#pragma once
#include <SFML/Network.hpp>
#include "entity.h"
#include "gameControl.h"
#include "filesystem/file/endian.h"
#include "socketContainer.h"
#include <thread>
#include <SFML/Graphics/RenderTexture.hpp>
struct playerSocket : socketContainer {
	human* player = nullptr;
	gameControl* screen = nullptr;
	bool authenticated = false;
	playerSocket(sf::TcpSocket* socket);

	//2 contexts and rendertextures, 1 for each thread. this is to prevent contexts from being created and deleted constantly
	byte thread0DoubleBufferIndex = 0;
	texture* doubleBuffer[2]{};
	//sf::RenderTexture doubleBuffer[2]{};
	//sf::Context contexts[2]{};
	//texture* lastRenderResult = nullptr;
	//sf::RenderTexture* lastRenderResult = nullptr;
	void processSocketInput();
	~playerSocket();
	bool shouldDisconnect = false;
	std::thread* sendRenderResultThread = nullptr;
	std::thread* sendPacketThread = nullptr;
};
void renderAsync(playerSocket* socket);
void sendRenderResultAsync(playerSocket* socket, nbtCompound* compound, nbtSerializer* s);
//void sendPacketAsync(playerSocket* socket);