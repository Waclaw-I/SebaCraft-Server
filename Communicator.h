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
	pPosition,
	pBullet
};

class Communicator
{
public:
	Communicator(int port, bool usePublic);

	bool getMessageSize(SOCKET & client, int & size);
	bool getPacketType(SOCKET & client, Packet & packetType);
	bool getMessage(SOCKET & client, std::string & message);

	bool sendMessageSize(SOCKET & client, int size);
	bool sendPacketType(SOCKET & client, Packet packetType);
	bool sendMessage(SOCKET & client, std::string & message);
	bool sendConsoleMessage(SOCKET & client, std::string & message);
	bool sendNewPlayerJoinedAlert(SOCKET & client, std::string & message);
	bool sendPlayerLeftAlert(SOCKET & client, std::string & message);
	bool sendInitialization(SOCKET & client, std::string & message);
	bool sendPlayersPosition(SOCKET & client, std::string & message);
	bool sendBulletInformation(SOCKET & client, std::string & message);

	SOCKADDR_IN addr;
	int addrLength = sizeof(addr);
	SOCKET sListen;

private:

	

};