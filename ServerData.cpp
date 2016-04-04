#include "ServerData.h"

ServerData::ServerData(int port, bool usePublic)
{
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 1);
	if (WSAStartup(DllVersion, &wsaData) != 0)
	{
		cout << "WinSock startup failed" << endl;
		exit(1);
	}

	if (usePublic) // server is open to public
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
	else // we are using localhost
		addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;

	sListen = socket(AF_INET, SOCK_STREAM, NULL);
	if (bind(sListen, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		cout << "Failed to bind the address to listening socket" << endl;
		exit(1);
	}
	if (listen(sListen, SOMAXCONN) == SOCKET_ERROR)
	{
		cout << "Failed to listen on listening socket" << endl;
		exit(1);
	}

	serverPtr = this;
}

int ServerData::getTotalConnections() { return totalConnections; }

bool ServerData::listenForNewConnection()
{
	SOCKET newConnection = accept(sListen, (SOCKADDR*)&addr, &addrLength);
	if (newConnection == 0)
	{
		cout << "Failed to accept connection" << endl;
		return false;
	}
	else
	{
		cout << "Client Connected! ID: " << totalConnections << endl;
		Connections[totalConnections] = newConnection;

		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandlerThread, (LPVOID)(totalConnections), NULL, NULL);
		string welcomeMessage = "Welcome on SebaCraft Server!";
		sendMessage(totalConnections, welcomeMessage);
		totalConnections++;
		return true;
	}
}

bool ServerData::processPacket(int ID, Packet packetType)
{
	switch (packetType)
	{
		case pMessage:
		{
			string message;
			if (!getMessage(ID, message)) return false;
			

			for (int i = 0; i < totalConnections; i++)
			{
				if (i == ID) continue;
				if (!sendMessage(i, message))
				{
					cout << "Failed to send message from client: " << ID << endl;
				}
			}
			cout << "Processed chat message from user " << ID;
			break;
		}
		default:
		{
			cout << "Unrecognized packet: " << packetType << endl;
			break;
		}
		return true;
	}
}

void ServerData::ClientHandlerThread(int ID) // index of the socket array
{
	Packet packetType;

	while (true)
	{
		if (!serverPtr->getPacketType(ID, packetType)) break;
		if (!serverPtr->processPacket(ID, packetType)) break;
	}

	cout << "Lost connection to client ID: " << ID << endl;
	closesocket(serverPtr->Connections[ID]);
}

bool ServerData::sendMessageSize(int ID, int size)
{
	int check = send(Connections[ID], (char*)& size, sizeof(int), NULL);
	if (check == SOCKET_ERROR) return false;
	return true;
}

bool ServerData::getMessageSize(int ID, int & size)
{
	int check = recv(Connections[ID], (char*)& size, sizeof(int), NULL);
	if (check == SOCKET_ERROR)
	{
		cout << "Failed to get message size" << endl;
		return false;
	}
	return true;
}

bool ServerData::sendPacketType(int ID, Packet packetType)
{
	int check = send(Connections[ID], (char*)& packetType, sizeof(Packet), NULL);
	if (check == SOCKET_ERROR) return false;
	else return true;
}

bool ServerData::getPacketType(int ID, Packet & packetType)
{
	int check = recv(Connections[ID], (char*)& packetType, sizeof(Packet), NULL);
	if (check == SOCKET_ERROR)
	{
		cout << "Couldn't get type of the packet" << endl;
		return false;
	}
	cout << "Packet type: " << packetType << endl;
	return true;
}

bool ServerData::sendMessage(int ID, string & message)
{
	if (!sendPacketType(ID, pMessage)) return false;
	int bufferLength = message.size();
	if (!sendMessageSize(ID, bufferLength)) return false;

	int check = send(Connections[ID], message.c_str(), bufferLength, NULL);
	if (check == SOCKET_ERROR) return false;
	else return true;
}

bool ServerData::getMessage(int ID, string & message)
{
	int bufferLength;
	if (!getMessageSize(ID, bufferLength))
	{
		cout << "Failed to get message" << endl;
		return false;
	}

	char * buffer = new char[bufferLength + 1];
	buffer[bufferLength] = '\0';

	int check = recv(Connections[ID], buffer, bufferLength, NULL);
	message = buffer;
	delete[] buffer;

	if (check == SOCKET_ERROR) return false;
	return true;

}