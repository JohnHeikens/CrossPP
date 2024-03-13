#pragma once
#include "playerSocket.h"
#include "constants.h"
struct server {
	sf::TcpListener listener = sf::TcpListener();
	sf::SocketSelector listenerSelector = sf::SocketSelector();
	std::vector<playerSocket*> clients = std::vector<playerSocket*>();
	bool everyoneSleeping = false;//to increase tick speed

	bool stopping = false;

	void execute();
	void renderClients();
	void stop();
	void tick();
	void updateToTime();
	void kick(playerSocket* socket);
	void addToServer(playerSocket* socket);
	microseconds msPerTick() const;
	human* findNearestPlayer(dimension* dimensionIn, cvec2& position);
	fp distanceToNearestPlayer(dimension* dimensionIn, cvec2& position);
	std::vector<human*> getPlayersInRadius(const dimension* dimensionIn, cvec2& position, cfp& radius);
};

extern server* currentServer;
void executeServer();
void createServerFromCurrentWorld();
playerSocket* listenForIncomingConnections();