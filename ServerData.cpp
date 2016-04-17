#include "ServerData.h"

#include <thread>

std::vector <Client *> ServerData::ClientsArray;
// constructor
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
// get elementary informations such as nickname and type of the ship and store it inside Client class object
Client * ServerData::initializeClient(SOCKET * socket)
{
	Packet packetType;
	if (!getPacketType(*socket, packetType)) std::cout << "No initialization packet received" << std::endl;
	// IT NEEDS TO BE DONE THIS WAY BECAUSE WE ARE NOW CREATING CLIENT OBJECT AND PROCESS_PACKET HAS THE CLIENT ARGUMENT
	switch (packetType)
	{
	case pInitialize: //receiving init packet from new client
	{
		if (!getMessage(*socket, Initializer)) return false; // we store received nickname (and other stuff later) in Initializer variable, which we use to create new Client dynamically
		shipType = atoi(Initializer.substr(0, 1).c_str()); // we are receiving both informations about ship model
		nickname = Initializer.substr(1, Initializer.size() - 1); // and player nickname
		break;
	}
	}

	// LATER ON, WE LL PARSE THE DATA FROM INITIALIZER STRING IN ORDER TO GET ALL INFORMATIONS INSIDE THIS METHOD
	Client * newClient = new Client(nickname, shipType, socket); // we create a new Client object which store such information as nick, id and socket

	return newClient;
} // 
// if there is no more room, send this message and close the socket
void ServerData::refuseToConnect()
{
	std::string message = "There is no room for more players on the server";
	std::cout << "Connection denied. No more room for another player" << std::endl;
	sendConsoleMessage(ClientsArray.back()->getSocket(), message);
	//closesocket(ClientsArray.back()->getSocket()); // somethings wrong with the last message from server to client - trash

	delete ClientsArray.back(); // this is working
	ClientsArray.pop_back();
}


void ServerData::listenForNewConnection()
{
	SOCKET * newConnection = new SOCKET; // we create a new socket dynamically
	*newConnection = accept(sListen, (SOCKADDR*)&addr, &addrLength); // newest connection is stored inside
	
	ClientsArray.push_back(initializeClient(newConnection)); // our pointer is now stored inside our vector of all active clients

	if (newConnection == 0) std::cout << "Failed to accept connection" << std::endl; // connecting was not successful
	
	else if (ClientsArray.size() > ConnectionLimit) refuseToConnect(); // IN CASE SOMEONE WOULD LIKE TO JOIN, BUT THERE IS ALREADY MAXIMUM AMOUNT OF PLAYERS
	
	else
	{
		std::cout << "Client Connected! Nickname: " << ClientsArray.back()->getNickname() << " ID: " << ClientsArray.back()->getId() << std::endl;

		
		

		std::string welcomeMessage = "Welcome on SebaCraft Server!";
		for (int i = 0; i < ClientsArray.size(); i++)
		{
			if (ClientsArray[i]->getSocket() == ClientsArray.back()->getSocket())
			{
				sendMessage(ClientsArray.back()->getSocket(), welcomeMessage); // a warm welcome message from the server. It ll be editable in the future
				// NOW WE NEED TO SEND INFORMATION ABOUT PLAYERS WHICH ARE ALREADY IN THE GAME
				for (int j = 0; j < ClientsArray.size(); j++)
				{
					if (ClientsArray[j]->getSocket() != ClientsArray.back()->getSocket())
					{
						std::string message = std::to_string(ClientsArray[j]->getShipType()) + ClientsArray[j]->getNickname() + "\t" + std::to_string(ClientsArray[j]->getId());
						sendNewPlayerJoinedAlert(ClientsArray.back()->getSocket(), message);// we are sending information to new player about every player already in the game
					}
				}
			}
			else
			{
				std::string message = std::to_string(ClientsArray.back()->getShipType()) + ClientsArray.back()->getNickname() + "\t" + std::to_string(ClientsArray.back()->getId());
				sendNewPlayerJoinedAlert(ClientsArray[i]->getSocket(), message); // we are sending information about new player to every one on the server
			}

		}
		std::cout << "Uruchamiam watki dla gracza " << ClientsArray.back()->getNickname() << std::endl;
		std::cout << "Stan jest rowny: " << ClientsArray.back()->getSendingReceiving() << std::endl;
		std::thread getDataThread(&GetDataFromClient, std::ref(*ClientsArray.back()));
		getDataThread.detach();
		std::thread sendDataThread(&SendDataToClient, std::ref(*ClientsArray.back()));
		sendDataThread.detach();
	}
}

bool ServerData::processPacket(Client & client, Packet packetType)
{
	switch (packetType)
	{
		case pMessage:
		{
			std::string message;
			if (!getMessage(client.getSocket(), message)) return false;

			for (int i = 0; i < ClientsArray.size(); i++) // SENDING TO OTHER PLAYERS
			{
				if (ClientsArray[i]->getSocket() == client.getSocket()) continue;
				if (!sendMessage(ClientsArray[i]->getSocket(), message))
				{
					std::cout << "Failed to send message from client: " << i << std::endl;
				}
			}
			std::cout << message << std::endl;
			break;
		}

		case pInitialize:
		{
			if (!getMessage(client.getSocket(), Initializer)) return false; // we store received nickname (and other stuff later) in Initializer variable, which we use to create new Client dynamically
			shipType = atoi(Initializer.substr(0, 1).c_str()); // we are receiving both informations about ship model
			nickname = Initializer.substr(1, Initializer.size() - 1); // and player nickname
			break;
		}

		case pPosition: // REVEIVING INFORMATION FROM CLIENT ABOUT HIS SHIP POSITION AND ROTATION
		{
			std::string position;
			if (!getMessage(client.getSocket(), position)) return false;
			int xParserPos = position.find("X");
			int yParserPos = position.find("Y");
			int rParserPos = position.find("R");

			double positionX = atof(position.substr(0, xParserPos ).c_str());
			double positionY = atof(position.substr(yParserPos + 1, yParserPos - xParserPos - 1).c_str());
			double rotation = atof(position.substr(yParserPos + 1, rParserPos - yParserPos - 1).c_str());

			client.getShipData().setPositionX(positionX);
			client.getShipData().setPositionY(positionY);
			client.getShipData().setRotation(rotation);
			break;
	
		}

		default:
		{
			std::cout << "Unrecognized packet: " << packetType << std::endl;
			break;
		}
		
		
	}
	return true;
}
void ServerData::SendDataToClient(Client & client)
{
	while (true)
	{
		//if (!client.getSendingReceiving())
		//{
			for (int i = 0; i < ClientsArray.size(); i++)
			{
				if (ClientsArray[i]->getId() != client.getId())
				{
					std::string position = std::to_string(ClientsArray[i]->getId()) + "X" + std::to_string(ClientsArray[i]->getShipData().getPositionX())
						+ "Y" + std::to_string(ClientsArray[i]->getShipData().getPositionY()) + "R" + std::to_string(ClientsArray[i]->getShipData().getRotation());
					serverPtr->sendPlayersPosition(client.getSocket(), position);
				}
			}
			//client.setSendingReceiving(true);
		//}
		
	}
}
void ServerData::GetDataFromClient(Client & client) // index of the socket array
{
	Packet packetType;
	while (true)
	{
		//if (client.getSendingReceiving())
		//{
			if (!serverPtr->getPacketType(client.getSocket(), packetType)) break;
			if (!serverPtr->processPacket(client, packetType)) break;
			//client.setSendingReceiving(false);
		//}
	}



	std::cout << "Lost connection to client ID: " << client.getId() << std::endl;
	
	closesocket(client.getSocket());
	int index;
	for (int i = 0; i < ClientsArray.size(); i++) // find position of this client inside our list
	{
		if (ClientsArray[i]->getId() == client.getId())
		{
			index = i;
			break;
		}
	}
	ClientsArray.erase(ClientsArray.begin() + index);
	for (int i = 0; i < ClientsArray.size(); i++)
	{
		serverPtr->sendPlayerLeftAlert(ClientsArray[i]->getSocket(), std::to_string(client.getId()));
	}
}


// ##################GET INFORMATION METHODS######################

bool ServerData::getPacketType(SOCKET & client, Packet & packetType)
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

bool ServerData::getMessageSize(SOCKET & client, int & size)
{
	int check = recv(client, (char*)& size, sizeof(int), NULL);
	if (check == SOCKET_ERROR)
	{
		std::cout << "Failed to get message size" << std::endl;
		return false;
	}
	return true;
}

bool ServerData::getMessage(SOCKET & client, std::string & message)
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

bool ServerData::sendPacketType(SOCKET & client, Packet packetType)
{
	int check = send(client, (char*)& packetType, sizeof(Packet), NULL);
	if (check == SOCKET_ERROR) return false;
	else return true;
}

bool ServerData::sendMessageSize(SOCKET & client, int size)
{
	int check = send(client, (char*)& size, sizeof(int), NULL);
	if (check == SOCKET_ERROR) return false;
	return true;
}

bool ServerData::sendMessage(SOCKET & client, std::string & message)
{
	if (!sendPacketType(client, pMessage)) return false;
	int bufferLength = message.size();
	if (!sendMessageSize(client, bufferLength)) return false;

	int check = send(client, message.c_str(), bufferLength, NULL);
	if (check == SOCKET_ERROR) return false;
	else return true;
}

bool ServerData::sendConsoleMessage(SOCKET & client, std::string & message)
{
	if (!sendPacketType(client, pConsole)) return false;
	int bufferLength = message.size();
	if (!sendMessageSize(client, bufferLength)) return false;

	int check = send(client, message.c_str(), bufferLength, NULL);
	if (check == SOCKET_ERROR) return false;
	else return true;
}

bool ServerData::sendNewPlayerJoinedAlert(SOCKET & client, std::string & message)
{
	if (!sendPacketType(client, pNewPlayer)) return false;
	int bufferLength = message.size();
	if (!sendMessageSize(client, bufferLength)) return false;

	int check = send(client, message.c_str(), bufferLength, NULL);
	if (check == SOCKET_ERROR) return false;
	else return true;
}

bool ServerData::sendPlayerLeftAlert(SOCKET & client, std::string & message)
{
	if (!sendPacketType(client, pRemovePlayer)) return false;
	int bufferLength = message.size();
	if (!sendMessageSize(client, bufferLength)) return false;

	int check = send(client, message.c_str(), bufferLength, NULL);
	if (check == SOCKET_ERROR) return false;
	else return true;
}

bool ServerData::sendInitialization(SOCKET & client, std::string & message)
{
	if (!sendPacketType(client, pInitialize)) return false;
	int bufferLength = message.size();
	if (!sendMessageSize(client, bufferLength)) return false;

	int check = send(client, message.c_str(), bufferLength, NULL);
	if (check == SOCKET_ERROR) return false;
	else return true;
}

bool ServerData::sendPlayersPosition(SOCKET & client, std::string & message)
{
	if (!sendPacketType(client, pPosition)) return false;
	int bufferLength = message.size();
	if (!sendMessageSize(client, bufferLength)) return false;

	int check = send(client, message.c_str(), bufferLength, NULL);
	if (check == SOCKET_ERROR) return false;
	else return true;
}


