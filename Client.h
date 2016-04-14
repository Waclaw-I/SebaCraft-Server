#pragma once
#include <Winsock2.h>
#include <string>

#include "ShipData.h"


class Client
{
public:

	Client(std::string nickname, int shipType, SOCKET * socket);
	~Client();

	void setNickname(std::string nickname);
	void setSocket(SOCKET * mySocket);
	void setShipType(int type);

	std::string getNickname();
	SOCKET & getSocket();
	int getId();
	int getShipType();


private:
	static int allTimeAmount;
	static int actualAmount;
	std::string nickname;
	int shipType;
	SOCKET * mySocket;
	int id;
	ShipData shipData;

	// INFORMATIONS ABOUT SHIP POSITION AND ROTATION


	
};