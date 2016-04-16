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


enum Packet
{
	pMessage,
	pConsole,
	pInitialize,
	pNewPlayer,
	pRemovePlayer,
	pPosition
};

class ServerData
{
private:

	int ConnectionLimit = 5; // up to n players!

	std::string Initializer;
	std::string nickname; // we ll get this from Initializer string
	int shipType; // along with information about chosen type of ship

	static std::vector <Client *> ClientsArray;

	SOCKADDR_IN addr;
	int addrLength = sizeof(addr);
	SOCKET sListen;


	bool sendMessageSize(SOCKET & client, int size);
	bool getMessageSize(SOCKET & client, int & size);

	bool sendPacketType(SOCKET & client, Packet packetType);
	bool getPacketType(SOCKET & client, Packet & packetType);

	bool sendMessage(SOCKET & client, std::string & message);
	bool sendConsoleMessage(SOCKET & client, std::string & message);
	bool sendNewPlayerJoinedAlert(SOCKET & client, std::string & message);
	bool sendPlayerLeftAlert(SOCKET & client, std::string & message);
	bool sendInitialization(SOCKET & client, std::string & message);
	bool sendPlayersPosition(SOCKET & client, std::string & message);
	bool getMessage(SOCKET & client, std::string & message);

	bool processPacket(Client & client, Packet packetType);

	static void GetDataFromClient(Client & client);
	static void SendDataToClient(Client & client);

public:

	ServerData(int port, bool usePublic);
	
	void listenForNewConnection();
	Client * initializeClient(SOCKET * socket);
	void refuseToConnect();


	
};

static ServerData * serverPtr;