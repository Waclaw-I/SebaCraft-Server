#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")

#pragma once
#include <iostream>
#include <Winsock2.h>
#include <string>

using namespace std;

class ServerData
{
private:
	static string ipAddress;
	static int portNumber;
	static WSADATA wsaData;
	static SOCKET mainSocket;
	static sockaddr_in service;

public:

	static string getIpAddress();
	static void setIpAddress(string ip);

	static int getPortNumber();
	static void setPortNumber(int port);

	static bool Initialize();
	static bool WaitForConnection();
};