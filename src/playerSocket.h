#pragma once
#include <SFML/Network.hpp>
#include "entity.h"
#include "gameControl.h"
#include "filesystem/file/endian.h"
#include "socketContainer.h"
#include <thread>
#include <SFML/Graphics/RenderTexture.hpp>
#include "math/graphics/doubleBuffer.h"
#include "math/graphics/video/videoEncoder.h"
struct playerSocket : socketContainer {
	human* player = nullptr;
	gameControl* screen = nullptr;
	bool authenticated = false;
	playerSocket(sf::TcpSocket* socket);

	//sf::RenderTexture doubleBuffer[2]{};
	//sf::Context contexts[2]{};
	//texture* lastRenderResult = nullptr;
	//sf::RenderTexture* lastRenderResult = nullptr;
	doubleBuffer buffer = doubleBuffer();
	videoEncoder encoder = videoEncoder();
	void processSocketInput();
	~playerSocket();
	bool shouldDisconnect = false;
	std::thread* sendRenderResultThread = nullptr;
	std::thread* sendPacketThread = nullptr;
	fp packetsReceivedPerSecond = 0;
	fp packetsSentPerSecond = 0;
};
void renderAsync(playerSocket* socket);
void sendRenderResultAsync(playerSocket* socket, nbtCompound* compound, nbtSerializer* s);
//void sendPacketAsync(playerSocket* socket);