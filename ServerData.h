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
	pInitialize
};

class ServerData
{
private:

	int ConnectionLimit = 2; // up to n players!

	std::string nicknameInitializer;

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
	bool getMessage(SOCKET & client, std::string & message);

	bool processPacket(SOCKET & client, Packet packetType);

	static void ClientHandlerThread(Client & client);

public:

	ServerData(int port, bool usePublic);
	
	void listenForNewConnection();
	Client * initializeClient(SOCKET * socket);
	void refuseToConnect();


	
};

static ServerData * serverPtr;