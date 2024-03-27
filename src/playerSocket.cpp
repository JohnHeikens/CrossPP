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
#include "fpng.h"
#include <thread>
#include <vector>
#include <SFML/Network/SocketSelector.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/CircleShape.hpp>
playerSocket::playerSocket(sf::TcpSocket *socket)
{
	screen = new gameControl(*this);
	this->s.socket = socket;
	write = false;

	//doubleBuffer[0].setActive(false);
	//doubleBuffer[1].setActive(false);

	//contexts[0].setActive(false);
	//contexts[1].setActive(false);

	//contexts[0] = std::move(sf::Context());

	ull num;
	// receive authentication packet.
	// wait for packet until timeout (fixes random connects pausing the server)
	// scope is for the selector
	{
		sf::SocketSelector selector;
		selector.add(*socket);
		// the packet should follow the connect shortly. we can wait safely, but one player can join at a time.
		// max 100 delay
		if (!selector.wait(sf::milliseconds(100)))
			return;
		s.receivePacket();
	}
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
	screen->player = player = new human(currentWorld->dimensions[(int)currentWorld->worldSpawnDimension], vec2(), *screen, playerName);
	crectangle2 &relativeHitbox = player->calculateHitBox();

	player->position = cvec2(currentWorld->worldSpawnPoint.x + 0.5, currentWorld->worldSpawnPoint.y) - relativeHitbox.pos0 + cvec2(relativeHitbox.size.x * -0.5, 0);
	player->newPosition = player->position;

	player->identifier = playerUUID;
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

void renderAsync(playerSocket *socket)
{
	// we need to instantiate a context for openGL rendering. we don't need to do anything with it.
	//texture &currentContext = socket->contexts[socket->thread0DoubleBufferIndex];
	//currentContext.setActive(true);
	//sf::RenderTexture &newRenderResult = socket->doubleBuffer[socket->thread0DoubleBufferIndex];


	// create the rendertexture connected to the active context
	//newRenderResult.create(socket->screen->rect.size.x, socket->screen->rect.size.y, sf::ContextSettings());
	//newRenderResult.setActive(true);
	// TODO: check if it needs clear()
	texture *& currentRenderTarget = socket->doubleBuffer[socket->thread0DoubleBufferIndex];
	
	if(!currentRenderTarget || currentRenderTarget->size != vect2<fsize_t>(socket->screen->rect.size))
	{
		if(currentRenderTarget)
		{
			delete currentRenderTarget;
		}
		currentRenderTarget = new texture(socket->screen->rect.size);
	}
	// we don't have to set the new render texture to active because its context is active already
	// newRenderResult.setActive(true);
	//newRenderResult.clear();
	socket->screen->render(cveci2(), *currentRenderTarget);
	//newRenderResult.setActive(false);
	//currentContext.setActive(false);
	//delete dummyTex;
	socket->screen->player->updateBodyParts(); // update body parts so the correct ear position can be obtained

	if (socket->sendRenderResultThread)
	{
		socket->sendRenderResultThread->join(); // wait until the last image has finished sending
		// socket->lastRenderResult->setActive(true);
		// delete socket->lastRenderResult;		// then delete the last render result
		delete socket->sendRenderResultThread;
	}
	// socket->lastRenderResult = newRenderResult;
	socket->write = true;

	// don't serialize this part async, as it doesn't take much time and might cause corruption when for example sounds are added mid-sending
	// send sounds

	vec2 earPosition = socket->screen->player->getHeadPosition();
	constexpr bool b = std::is_base_of_v<arraynd<color, 2>, texture>;
	socket->serialize(earPosition);

	size_t soundCount = socket->screen->dataToSend.size();
	socket->serialize(soundCount);
	for (auto &data : socket->screen->dataToSend)
	{
		data.serialize(*socket);
	}
	socket->screen->dataToSend.clear();

	//newRenderResult.setActive(false);
	socket->thread0DoubleBufferIndex = 1 - socket->thread0DoubleBufferIndex;
	socket->sendRenderResultThread = new std::thread(sendRenderResultAsync, socket);

	sf::SocketSelector selector;
	selector.add(*socket->s.socket);
	selector.wait(sf::microseconds(1));
	// we don't wait for input, we just process the input in the next frame. too bad, it would cause server lag
	while (selector.isReady(*socket->s.socket))
	{ // the wait() of the socket is called in the main server loop once a tick
		// finally, send the packet with all the data of this render cycle to the player
		if (is_in(socket->s.receivePacket(), sf::Socket::Status::Disconnected, sf::Socket::Status::Error))
		{ // client disconnected?
			socket->shouldDisconnect = true;
			return;
		}
		else
		{
			// selector should continue looking if the socket is still ready
			socket->write = false;

			// read input while sending
			socket->screen->mostRecentInput.serialize(*socket);
			vect2<fsize_t> newScreenSize;
			socket->serialize(newScreenSize);
			if (veci2(newScreenSize) != socket->screen->rect.size)
			{
				socket->screen->layout(crectanglei2(veci2(), veci2(newScreenSize)));
			}
			// socket->s.socket->setBlocking(true);
			socket->screen->addClientInput(socket->screen->mostRecentInput);
			socket->screen->processInput();
			selector.wait(sf::microseconds(1));
		}
	}
}

void sendRenderResultAsync(playerSocket *socket)
{
	byte thread1DoubleBufferIndex = 1 - socket->thread0DoubleBufferIndex;
	//sf::Context &currentContext = socket->contexts[thread1DoubleBufferIndex];
	texture* &currentRenderResult = socket->doubleBuffer[thread1DoubleBufferIndex];
	// serialize screen and finally, send the packet

	// TODO: serialize the screen in 'sendrenderresultasync'. this is hard because s->write = false
	// we don't use the normal 'serialize' function so we don't have to use the 'write' boolean
	// std::stringstream compressedScreenPacket = std::stringstream();
	// sf::RenderTexture tex = sf::RenderTexture();
	// tex.create(socket->lastRenderResult->size.x, socket->lastRenderResult->size.y);
	// tex.clear();
	// sf::CircleShape shape(500);
	// shape.setPosition(sf::Vector2f(300, 300));
	// tex.draw(shape);

	// currentContext.setActive(true);
	//currentRenderResult.setActive(true); // this automatically activates the context of the render result
	// currentRenderResult->clear();
	//currentRenderResult.display();
	//sf::Image i = currentRenderResult.getTexture().copyToImage();
	//currentRenderResult.setActive(false);
	// currentContext.setActive(false);

	//uint pixelCount = i.getSize().x * i.getSize().y;

	std::vector<byte> compressedBuf;
	typedef colortn<byte, 3> color3;
	color3 *colorsWithoutAlpha = new color3[currentRenderResult->size.volume()];
	const color3 c = color3();
	color3 *ptr = colorsWithoutAlpha;
	//color *imagePtr = (color *)i.getPixelsPtr();
	for (const color& c : *currentRenderResult)
	{
		*ptr++ = color3(c);
	}

	// std::copy(socket->lastRenderResult->baseArray, socket->lastRenderResult->baseArray + socket->lastRenderResult->size.volume(), colorsWithoutAlpha);
	fpng::fpng_encode_image_to_memory(colorsWithoutAlpha, currentRenderResult->size.x, currentRenderResult->size.y, rgbColorChannelCount, compressedBuf);
	delete[] colorsWithoutAlpha;
	// color* ptr = socket->lastRenderResult->baseArray;
	// std::vector<colorChannel> channels[rgbColorChannelCount]{};
	//
	////for (int i = 0; i < rgbColorChannelCount; i++) {
	////	channels[i] = new colorChannel[];
	////}
	////[socket->screen->rect.size.volume()] ;
	// for (int i = 0; i < socket->screen->rect.size.volume(); i++, ptr++) {
	//	channels[0].push_back(ptr->axis[0]);//b
	//	channels[1].push_back(ptr->axis[1]);//g
	//	channels[2].push_back(ptr->axis[2]);//r
	//	//compressedScreenPacket..write(ptr->r(), ptr->g(), ptr->b())
	//
	// }
	// for (int channelIndex = 0; channelIndex < rgbColorChannelCount; channelIndex++) {
	//	size_t s = channels[channelIndex].size();
	//	socket->s.write((const char*)&s, sizeof(size_t));
	//	socket->s.write((const char*)&(*channels[channelIndex].begin()), channels[channelIndex].size());
	// }

	// socket->s.write((char*)(color::channelType*)socket->lastRenderResult->baseArray, socket->screen->rect.size.volume() * bgraColorChannelCount * sizeof(color::channelType));

	size_t s = compressedBuf.size(); // TODO: video streaming
	// TODO: fix endian
	socket->s.write((const char *)&s, sizeof(size_t));
	socket->s.write((const char *)&(*compressedBuf.begin()), compressedBuf.size());
	if (socket->sendPacketThread)
	{
		socket->sendPacketThread->join();
		delete socket->sendPacketThread;
	}
	// socket->sendPacketThread = new std::thread(sendPacketAsync, socket);
	socket->sendPacketThread = new std::thread([socket]()
											   { socket->s.sendPacket(); });
}

// void sendPacketAsync(playerSocket* socket) {
//
//	socket->s.sendPacket();
// }