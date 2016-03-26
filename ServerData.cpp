#include "ServerData.h"

string ServerData::ipAddress = "127.0.0.1";
int ServerData::portNumber;
WSADATA ServerData::wsaData;
SOCKET ServerData::mainSocket;
sockaddr_in ServerData::service;

string ServerData::getIpAddress() { return ipAddress; }
int ServerData::getPortNumber() { return portNumber; }

bool ServerData::Initialize()
{
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != NO_ERROR)
	{
		cout << "Initialization error" << endl;
		return false;
	}

	mainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // korzystamy z TCP
	if (mainSocket == INVALID_SOCKET)
	{
		cout << "Error creating socket: " << WSAGetLastError() << endl; // zwraca kod ostatniego bledu
		WSACleanup(); // sprzata po uzyciu Winsock
		return false;
	}

	cout << "Insert port number: ";
	cin >> portNumber;

	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr(ipAddress.c_str()); // ten sam komputer, poki co
	service.sin_port = htons(portNumber); // port, ktorego bedziemy uzywac

	if (bind(mainSocket, (SOCKADDR *)& service, sizeof(service)) == SOCKET_ERROR)
	{
		cout << "Binding port was not succesfull." << endl;
		closesocket(mainSocket);
		return false;
	}

	if (listen(mainSocket, 1) == SOCKET_ERROR)
	{
		cout << "Error listening on socket" << endl; // nasluchiwanie portu
		return false;
	}

	return true;
}

bool ServerData::WaitForConnection()
{
	SOCKET acceptSocket = SOCKET_ERROR;
	cout << "Waiting for client to connect..." << endl;

	while (acceptSocket == SOCKET_ERROR) // dopoki nie nastapi polaczenie
	{
		acceptSocket = accept(mainSocket, NULL, NULL);
	}
	cout << "Client connected!" << endl;
	mainSocket = acceptSocket; // zastepujemy uchwyt gniazda tym otrzymanym z accept 
	return true;
}