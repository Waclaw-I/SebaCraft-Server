#pragma once
#include <Winsock2.h>
#include <string>
#include <vector>

#include "ShipData.h"


class Client
{
public:

	Client(std::string nickname, int shipType, SOCKET * socket);
	~Client();

	void setNickname(std::string nickname);
	void setSocket(SOCKET * mySocket);
	void setShipType(int type);
	void setOnline(bool state);

	std::vector<Client *> & getClientsArray();


	std::string getNickname();
	SOCKET & getSocket();
	int getId();
	int getShipType();
	bool isOnline();


	ShipData & getShipData();


private:
	static int allTimeAmount;
	static int actualAmount;
	std::string nickname;
	int shipType;
	SOCKET * mySocket;
	int id;
	ShipData shipData;
	std::vector <Client *> clientsArray;

	bool Online;

	// INFORMATIONS ABOUT SHIP POSITION AND ROTATION


	
};