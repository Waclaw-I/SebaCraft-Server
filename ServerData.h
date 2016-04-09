#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")

#pragma once
#include <iostream>
#include <Winsock2.h>
#include <string>
#include <thread>


enum Packet
{
	pMessage,
	pConsole
};

class ServerData
{
private:

	SOCKET Connections[3]; // up to 8 players!
	int totalConnections = 0;
	SOCKADDR_IN addr;
	int addrLength = sizeof(addr);
	SOCKET sListen;


	bool sendMessageSize(int ID, int size);
	bool getMessageSize(int ID, int & size);

	bool sendPacketType(int ID, Packet packetType);
	bool getPacketType(int ID, Packet & packetType);

	bool sendMessage(int ID, std::string & message);
	bool sendConsoleMessage(int ID, std::string & message);
	bool getMessage(int ID, std::string & message);

	bool processPacket(int ID, Packet packetType);

	static void ClientHandlerThread(int ID);

public:

	ServerData(int port, bool usePublic);
	
	bool listenForNewConnection();

	int getTotalConnections();
};

static ServerData * serverPtr;