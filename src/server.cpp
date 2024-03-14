#include "server.h"
#include "human.h"
#include "world.h"
#include "dimension.h"
#include "end.h"
#include <thread>
#include "client.h"
#include "benchMark.h"
#include <optimization/stableTickLoop.h>
#include "cpuUsageID.h"
#include <future>
server* currentServer = nullptr;
std::thread* serverThread = nullptr;

void server::execute()
{
	// bind the listener to a port
	if (listener.listen(defaultPort) != sf::Socket::Done)
	{
		// error...
	}
	listenerSelector.add(listener);


	stableLoop loop = stableLoop(1000000 / 60);
	//connectionManagerThread = new std::thread(listenForIncomingConnections);

	std::future<playerSocket*> newPlayerSocket = std::async(&listenForIncomingConnections);

	while (!stopping) {//server loop
		loop.waitTillNextLoopTime();

		updateToTime();

		//render
		renderClients();
		currentBenchmark->addBenchmarkPoint(cpuUsageID::networking);

		for (int i = 0; i < clients.size();) {
			if (clients[i]->shouldDisconnect) {
				kick(clients[i]);
			}
			else {
				i++;
			}
		}

		//accept connection
		if (newPlayerSocket._Is_ready()) {
			if (playerSocket* newSocket = newPlayerSocket.get()) {
				addToServer(newSocket);
			}
			newPlayerSocket = std::async(&listenForIncomingConnections);
		}
		//connectionManagerThread->join();
		//delete connectionManagerThread;
		//connectionManagerThread = new std::thread(listenForIncomingConnections);




		currentBenchmark->addBenchmarkPoint(cpuUsageID::miscellaneous);

	}
	//delete connectionManagerThread;
	const std::vector<playerSocket*> copy = clients;
	for (playerSocket* const& c : copy) {
		kick(c);
	}
	//stopping
	currentWorld->serialize(true);
	delete currentWorld;
	currentWorld = nullptr;
	delete this;
	currentServer = nullptr;
}

void server::renderClients()
{
	currentBenchmark->addBenchmarkPoint(cpuUsageID::drawing);

	std::vector<std::thread*> threads = std::vector<std::thread*>();
	for (auto c : clients) {
		threads.push_back(new std::thread(renderAsync, c));
	}
	//wait until all screens have rendered
	for (auto t : threads) {
		t->join();
		delete t;
	}
	currentBenchmark->removeOldBenchmarks();//the rendering time will be displayed in the next render session
	currentBenchmark->addBenchmarkPoint(cpuUsageID::miscellaneous);
}

void server::stop()
{
	stopping = true;
	//wait until the server has stopped
	serverThread->join();
}

void server::tick()
{
	currentBenchmark->addBenchmarkPoint(cpuUsageID::chunkLoading);



	constexpr fp playerLoadDistance = 0x20;//the distance which should always be loaded in around the player, with or without a camera looking at it

	//visiblerange * 2 because the camera can load in 2x further
	//keep chunks around the player loaded. the visible range will only update every frame, but that doesn't matter much
	bool inEnd = false;
	everyoneSleeping = true;
	for (const auto* client : clients)
	{
		if (!inEnd && client->player->dimensionIn->identifier == dimensionID::end) {
			inEnd = true;
		}
		if (!client->player->sleeping) {
			everyoneSleeping = false;
		}
		client->player->dimensionIn->keepPlayerLoaded(dimension::getTouchingChunkCoordinateRange(crectangle2(client->screen->cameraPosition - client->screen->visibleRange, client->screen->visibleRange * 2)));
		client->player->dimensionIn->keepPlayerLoaded(dimension::getTouchingChunkCoordinateRadius(client->player->position, playerLoadDistance));
	}
	if (inEnd)
	{
		end* currentEnd = (end*)currentWorld->dimensions[(int)dimensionID::end];
		if (currentEnd->dragonAlive)
		{
			currentEnd->keepPlayerLoaded(dimension::getTouchingChunkCoordinateRange(crectangle2(-mainEndIslandMaxRadius, 0, mainEndIslandMaxRadius * 2, 0x100)));
		}
	}
	currentBenchmark->addBenchmarkPoint(cpuUsageID::miscellaneous);
	currentWorld->tick();

	//also set them to false if the world does not have focus, so it will not be placing random blocks
	for (auto c : clients) {
		c->screen->clearTemporaryData();
		fillAllElements(c->screen->clickedFocused, false);
		//fillAllElements(c->screen->holding, false);
	}


	//rightClicked = false;
	//leftClicked = false;
	//middleClicked = false;
	//rightClickReleased = false;
	//leftClickReleased = false;
	//middleClickReleased = false;
	//holdingLeftClick = false;
	//holdingRightClick = false;
	//holdingMiddleClick = false;
}

void server::updateToTime()
{
	microseconds s = getmicroseconds();

	//first update the transform, so all entities know the drawing positions already
	int tickCount = 0;
	cint maxTicksPerFrame = 0x4;//to avoid an endless lag loop
	while (s > lastTickTimeMicroseconds + msPerTick())
	{
		if (tickCount >= maxTicksPerFrame)
		{
			lastTickTimeMicroseconds = s;
			lastTickTime = microsectosec(lastTickTimeMicroseconds);
			break;
		}
		tick();
		lastTickTimeMicroseconds += msPerTick();
		lastTickTime = microsectosec(lastTickTimeMicroseconds);
		tickCount++;
	}
}

void server::kick(playerSocket* socket)
{
	socket->player->serialize(true);
	socket->player->despawn = true;
	//selector.remove(*(socket->s.socket));
	clients.erase(find(clients, socket));
	socket->s.socket->disconnect();
}

void server::addToServer(playerSocket* socket)
{
	socket->player->addToWorld(socket->player->identifier);
	clients.push_back(socket);
}

microseconds server::msPerTick() const
{
	return everyoneSleeping ? microSecondsPerTick / 0x10 : microSecondsPerTick;
}

human* server::findNearestPlayer(dimension* dimensionIn, cvec2& position)
{
	human* nearestPlayer = nullptr;
	fp nearestDistanceSquared = INFINITY;
	for (auto p : clients) {
		if (p->player->dimensionIn == dimensionIn) {
			fp dist = (p->player->position - position).lengthSquared();
			if (dist < nearestDistanceSquared)
			{
				nearestDistanceSquared = dist;
				nearestPlayer = p->player;
			}
		}
	}

	return nearestPlayer;
}

fp server::distanceToNearestPlayer(dimension* dimensionIn, cvec2& position)
{
	const human* nearestPlayer = findNearestPlayer(dimensionIn, position);
	return nearestPlayer ? (nearestPlayer->position - position).length() : INFINITY;
}

std::vector<human*> server::getPlayersInRadius(const dimension* dimensionIn, cvec2& position, cfp& radius)
{
	cfp& radiusSquared = radius * radius;
	std::vector<human*> result = std::vector<human*>();
	for (auto p : clients) {
		human* const& h = p->player;
		if (h->dimensionIn == dimensionIn) {
			if ((h->position - position).lengthSquared() < radiusSquared)
			{
				result.push_back(h);
			}
		}
	}
	return result;
}

void executeServer()
{
	currentServer->execute();
}

void createServerFromCurrentWorld()
{
	currentServer = new server();
	serverThread = new std::thread(executeServer);//starts the server
	currentClient->connectToServer(serverData());
}

playerSocket* listenForIncomingConnections()
{
	//seconds tickTimeLeft = lastTickTime + microsectosec(msPerTick()) - getSeconds();
//dummy time

	if (currentServer->listenerSelector.wait(sf::microseconds(1))) {

		// received something
		//if (selector.isReady(listener))
		//{
			// accept a new connection
		sf::TcpSocket* clientSocket = new sf::TcpSocket();
		if (currentServer->listener.accept(*clientSocket) != sf::Socket::Done)
		{
			// error...
		}


		playerSocket* socket = new playerSocket(clientSocket);

		if (socket->authenticated)
		{
			return socket;
			//selector.add(*clientSocket);
		}
		else {
			//this client didn't authenticate
			delete socket;
		}
		//}
	}
	return nullptr;
	//for (auto c : clients) {
	//	if (selector.isReady(*c->s.socket)) {
	//		//the client has sent us data
	//		c->processSocketInput();
	//	}
	//}
}
