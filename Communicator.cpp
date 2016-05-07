#include "Communicator.h"

Communicator::Communicator(int port, bool usePublic)
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
		 //addr.sin_addr.s_addr = inet_addr("25.0.252.153");
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
}

// ##################GET INFORMATION METHODS######################

bool Communicator::getPacketType(SOCKET & client, Packet & packetType)
{
	int check = recv(client, (char*)& packetType, sizeof(Packet), NULL);
	if (check == SOCKET_ERROR)
	{
		std::cout << "Couldn't get type of the packet" << std::endl;
		return false;
	}
	if (packetType == 0) std::cout << "Packet type: ChatMessage" << std::endl;
	if (packetType == 1) std::cout << "Packet type: ConsoleError" << std::endl;
	if (packetType == 2) std::cout << "Packet type: Initializer" << std::endl;

	return true;
}

bool Communicator::getMessageSize(SOCKET & client, int & size)
{
	int check = recv(client, (char*)& size, sizeof(int), NULL);
	if (check == SOCKET_ERROR)
	{
		std::cout << "Failed to get message size" << std::endl;
		return false;
	}
	return true;
}

bool Communicator::getMessage(SOCKET & client, std::string & message)
{
	int bufferLength;
	if (!getMessageSize(client, bufferLength))
	{
		std::cout << "Failed to get message" << std::endl;
		return false;
	}

	char * buffer = new char[bufferLength + 1];
	buffer[bufferLength] = '\0';

	int check = recv(client, buffer, bufferLength, NULL);
	message = buffer;
	delete[] buffer;

	if (check == SOCKET_ERROR) return false;
	return true;

}

// ##################SEND INFORMATION METHODS######################

bool Communicator::sendPacketType(SOCKET & client, Packet packetType)
{
	int check = send(client, (char*)& packetType, sizeof(Packet), NULL);
	if (check == SOCKET_ERROR) return false;
	else return true;
}

bool Communicator::sendMessageSize(SOCKET & client, int size)
{
	int check = send(client, (char*)& size, sizeof(int), NULL);
	if (check == SOCKET_ERROR) return false;
	return true;
}

bool Communicator::sendMessage(SOCKET & client, std::string & message)
{
	if (!sendPacketType(client, pMessage)) return false;
	int bufferLength = message.size();
	if (!sendMessageSize(client, bufferLength)) return false;

	int check = send(client, message.c_str(), bufferLength, NULL);
	if (check == SOCKET_ERROR) return false;
	else return true;
}

bool Communicator::sendConsoleMessage(SOCKET & client, std::string & message)
{
	if (!sendPacketType(client, pConsole)) return false;
	int bufferLength = message.size();
	if (!sendMessageSize(client, bufferLength)) return false;

	int check = send(client, message.c_str(), bufferLength, NULL);
	if (check == SOCKET_ERROR) return false;
	else return true;
}

bool Communicator::sendNewPlayerJoinedAlert(SOCKET & client, std::string & message)
{
	if (!sendPacketType(client, pNewPlayer)) return false;
	int bufferLength = message.size();
	if (!sendMessageSize(client, bufferLength)) return false;

	int check = send(client, message.c_str(), bufferLength, NULL);
	if (check == SOCKET_ERROR) return false;
	else return true;
}

bool Communicator::sendPlayerLeftAlert(SOCKET & client, std::string & message)
{
	if (!sendPacketType(client, pRemovePlayer)) return false;
	int bufferLength = message.size();
	if (!sendMessageSize(client, bufferLength)) return false;

	int check = send(client, message.c_str(), bufferLength, NULL);
	if (check == SOCKET_ERROR) return false;
	else return true;
}

bool Communicator::sendInitialization(SOCKET & client, std::string & message)
{
	if (!sendPacketType(client, pInitialize)) return false;
	int bufferLength = message.size();
	if (!sendMessageSize(client, bufferLength)) return false;

	int check = send(client, message.c_str(), bufferLength, NULL);
	if (check == SOCKET_ERROR) return false;
	else return true;
}

bool Communicator::sendPlayersPosition(SOCKET & client, std::string & message)
{
	if (!sendPacketType(client, pPosition)) return false;
	int bufferLength = message.size();
	if (!sendMessageSize(client, bufferLength)) return false;

	int check = send(client, message.c_str(), bufferLength, NULL);
	if (check == SOCKET_ERROR) return false;
	else return true;
}
