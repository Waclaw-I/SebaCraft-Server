#include "Client.h"

#include <iostream>

int Client::allTimeAmount = 0;
int Client::actualAmount = 0;

void Client::setNickname(std::string nickname) { this->nickname = nickname; }
void Client::setSocket(SOCKET * mySocket) { this->mySocket = mySocket; }

std::string Client::getNickname() { return this->nickname; }
SOCKET & Client::getSocket() { return *this->mySocket; }
int Client::getId() { return this->id; }

Client::Client(std::string nickname, SOCKET * socket)
{
	setNickname(nickname);
	setSocket(socket);
	id = allTimeAmount++;
	actualAmount++;
}

Client::~Client()
{
	std::cout << this->nickname << ", ID: " << this->id << " got disconnected" << std::endl;
	actualAmount--;
}