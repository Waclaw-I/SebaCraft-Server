#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")

#pragma once
#include <iostream>
#include <Winsock2.h>
#include <string>
#include <thread>
#include <vector>

#include "Client.h"
#include "Communicator.h"


class ServerData
{

public:

	ServerData(int port, bool usePublic);

	static bool isClientsArrayChanging();
	static void setClientsArrayIsChanging(bool state);

	void listenForNewConnection();
	Client * initializeClient(SOCKET * socket);
	void refuseToConnect();

private:

	Communicator * communicator;

	int ConnectionLimit = 5; // up to n players!

	std::string Initializer;
	std::string nickname; // we ll get this from Initializer string
	int shipType; // along with information about chosen type of ship

	static std::vector <Client *> ClientsArray;
	static bool ClientsArrayIsChanging;




	bool processPacket(Client & client, Packet packetType);

	static void GetDataFromClient(Client & client);
	static void SendDataToClient(Client & client);


};

static ServerData * serverPtr;