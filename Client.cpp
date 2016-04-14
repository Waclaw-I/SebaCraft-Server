#include "Client.h"

#include <iostream>

int Client::allTimeAmount = 0;
int Client::actualAmount = 0;

void Client::setNickname(std::string nickname) { this->nickname = nickname; }
void Client::setSocket(SOCKET * mySocket) { this->mySocket = mySocket; }
void Client::setShipType(int type) { this->shipType = type; }

std::string Client::getNickname() { return this->nickname; }
SOCKET & Client::getSocket() { return *this->mySocket; }
int Client::getId() { return this->id; }
int Client::getShipType() { return this->shipType; }

Client::Client(std::string nickname, int shipType, SOCKET * socket)
{
	setNickname(nickname);
	setSocket(socket);
	setShipType(shipType);
	id = allTimeAmount++;
	actualAmount++;

}

Client::~Client()
{
	std::cout << this->nickname << ", ID: " << this->id << " got disconnected" << std::endl;
	actualAmount--;
}