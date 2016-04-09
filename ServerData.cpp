#include "ServerData.h"

#include <thread>


ServerData::ServerData(int port, bool usePublic)
{
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 1);
	if (WSAStartup(DllVersion, &wsaData) != 0)
	{
		std::cout << "WinSock startup failed" << std::endl;
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
		std::cout << "Failed to bind the address to listening socket" << std::endl;
		exit(1);
	}
	if (listen(sListen, SOMAXCONN) == SOCKET_ERROR)
	{
		std::cout << "Failed to listen on listening socket" << std::endl;
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
		std::cout << "Failed to accept connection" << std::endl;
		return false;
	}
	else if (totalConnections >= 2) // IN CASE SOMEONE WOULD LIKE TO JOIN, BUT THERE IS ALREADY MAXIMUM AMOUNT OF PLAYERS
	{
		std::string message = "There is no room for more players on the server";
		std::cout << "Connection denied. No more room for another player" << std::endl;
		Connections[2] = newConnection;
		sendConsoleMessage(2, message);
	}
	else
	{
		std::cout << "Client Connected! ID: " << totalConnections << std::endl;
		Connections[totalConnections] = newConnection;

		//std::thread newClient(ClientHandlerThread, totalConnections);
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandlerThread, (LPVOID)(totalConnections), NULL, NULL);
		std::string welcomeMessage = "Welcome on SebaCraft Server!";
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
			std::string message;
			if (!getMessage(ID, message)) return false;

			message = std::to_string(ID) + message; // adding id in the first position to recognize player
			

			for (int i = 0; i < totalConnections; i++)
			{
				if (i == ID) continue;
				if (!sendMessage(i, message))
				{
					std::cout << "Failed to send message from client: " << ID << std::endl;
				}
			}
			std::cout << ID << ": " << message.substr(1, message.size() - 1) << std::endl; // we are displaying everything on the server console
			break;
		}
		default:
		{
			std::cout << "Unrecognized packet: " << packetType << std::endl;
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

	std::cout << "Lost connection to client ID: " << ID << std::endl;
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
		std::cout << "Failed to get message size" << std::endl;
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
		std::cout << "Couldn't get type of the packet" << std::endl;
		return false;
	}
	if (packetType == 0) std::cout << "Packet type: ChatMessage" << std::endl;

	return true;
}

bool ServerData::sendMessage(int ID, std::string & message)
{
	if (!sendPacketType(ID, pMessage)) return false;
	int bufferLength = message.size();
	if (!sendMessageSize(ID, bufferLength)) return false;

	int check = send(Connections[ID], message.c_str(), bufferLength, NULL);
	if (check == SOCKET_ERROR) return false;
	else return true;
}

bool ServerData::sendConsoleMessage(int ID, std::string & message)
{
	if (!sendPacketType(ID, pConsole)) return false;
	int bufferLength = message.size();
	if (!sendMessageSize(ID, bufferLength)) return false;

	int check = send(Connections[ID], message.c_str(), bufferLength, NULL);
	if (check == SOCKET_ERROR) return false;
	else return true;
}

bool ServerData::getMessage(int ID, std::string & message)
{
	int bufferLength;
	if (!getMessageSize(ID, bufferLength))
	{
		std::cout << "Failed to get message" << std::endl;
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