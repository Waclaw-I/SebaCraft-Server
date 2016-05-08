#include "ServerData.h"

#include <thread>

std::vector <Client *> ServerData::ClientsArray;
bool ServerData::ClientsArrayIsChanging = false;

bool ServerData::isClientsArrayChanging() { return ClientsArrayIsChanging; }
void ServerData::setClientsArrayIsChanging(bool state) { ClientsArrayIsChanging = state; }

// constructor
ServerData::ServerData(int port, bool usePublic)
{
	communicator = new Communicator(port, usePublic);

	serverPtr = this;

} 

// get elementary informations such as nickname and type of the ship and store it inside Client class object
Client * ServerData::initializeClient(SOCKET * socket)
{
	Packet packetType;
	if (!communicator->getPacketType(*socket, packetType)) std::cout << "No initialization packet received" << std::endl;
	// IT NEEDS TO BE DONE THIS WAY BECAUSE WE ARE NOW CREATING CLIENT OBJECT AND PROCESS_PACKET HAS THE CLIENT ARGUMENT
	switch (packetType)
	{
	case pInitialize: //receiving init packet from new client
	{
		if (!communicator->getMessage(*socket, Initializer)) return false; // we store received nickname (and other stuff later) in Initializer variable, which we use to create new Client dynamically
		shipType = atoi(Initializer.substr(0, 1).c_str()); // we are receiving both informations about ship model
		nickname = Initializer.substr(1, Initializer.size() - 1); // and player nickname
		break;
	}
	}

	// LATER ON, WE LL PARSE THE DATA FROM INITIALIZER STRING IN ORDER TO GET ALL INFORMATIONS INSIDE THIS METHOD
	Client * newClient = new Client(nickname, shipType, socket); // we create a new Client object which store such information as nick, id and socket

	return newClient;
} 

// if there is no more room, send this message and close the socket
void ServerData::refuseToConnect()
{
	std::string message = "There is no room for more players on the server";
	std::cout << "Connection denied. No more room for another player" << std::endl;
	communicator->sendConsoleMessage(ClientsArray.back()->getSocket(), message);
	//closesocket(ClientsArray.back()->getSocket()); // somethings wrong with the last message from server to client - trash

	delete ClientsArray.back(); // this is working
	ClientsArray.pop_back();
}


void ServerData::listenForNewConnection()
{
	SOCKET * newConnection = new SOCKET; // we create a new socket dynamically
	*newConnection = accept(communicator->sListen, (SOCKADDR*)&communicator->addr, &communicator->addrLength); // newest connection is stored inside
	
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
				communicator->sendMessage(ClientsArray.back()->getSocket(), welcomeMessage); // a warm welcome message from the server. It ll be editable in the future
				// NOW WE NEED TO SEND INFORMATION ABOUT PLAYERS WHICH ARE ALREADY IN THE GAME
				for (int j = 0; j < ClientsArray.size(); j++)
				{
					if (ClientsArray[j]->getSocket() != ClientsArray.back()->getSocket())
					{
						std::string message = std::to_string(ClientsArray[j]->getShipType()) + ClientsArray[j]->getNickname() + "\t" + std::to_string(ClientsArray[j]->getId());
						communicator->sendNewPlayerJoinedAlert(ClientsArray.back()->getSocket(), message);// we are sending information to new player about every player already in the game
						ClientsArray.back()->getClientsArray().push_back(ClientsArray[j]); // we are filling up private array of clients of every client
					}
				}
			}
			else
			{
				std::string message = std::to_string(ClientsArray.back()->getShipType()) + ClientsArray.back()->getNickname() + "\t" + std::to_string(ClientsArray.back()->getId());
				communicator->sendNewPlayerJoinedAlert(ClientsArray[i]->getSocket(), message); // we are sending information about new player to every one on the server
				ClientsArray[i]->getClientsArray().push_back(ClientsArray.back());
			}
		}


		std::cout << "Uruchamiam watki dla gracza " << ClientsArray.back()->getNickname() << std::endl;
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
			if (!communicator->getMessage(client.getSocket(), message)) return false;

			for (int i = 0; i < ClientsArray.size(); i++) // SENDING TO OTHER PLAYERS
			{
				if (ClientsArray[i]->getSocket() == client.getSocket()) continue;
				if (!communicator->sendMessage(ClientsArray[i]->getSocket(), message))
				{
					std::cout << "Failed to send message from client: " << i << std::endl;
				}
			}
			std::cout << message << std::endl;
			break;
		}

		case pInitialize:
		{
			if (!communicator->getMessage(client.getSocket(), Initializer)) return false; // we store received nickname (and other stuff later) in Initializer variable, which we use to create new Client dynamically
			shipType = atoi(Initializer.substr(0, 1).c_str()); // we are receiving both informations about ship model
			nickname = Initializer.substr(1, Initializer.size() - 1); // and player nickname
			break;
		}

		case pRemovePlayer:
		{
			std::string msg;
			if (!communicator->getMessage(client.getSocket(), msg)) return false;
			client.setOnline(false);
			std::cout << "PLAYER " << client.getNickname() << " HAS LEFT THE GAME" << std::endl;
		}

		case pPosition: // REVEIVING INFORMATION FROM CLIENT ABOUT HIS SHIP POSITION AND ROTATION
		{
			std::string position;
			if (!communicator->getMessage(client.getSocket(), position)) return false;
			int xParserPos = position.find("X");
			int yParserPos = position.find("Y");
			int rParserPos = position.find("R");

			double positionX = atof(position.substr(0, xParserPos ).c_str());
			double positionY = atof(position.substr(xParserPos + 1, yParserPos - xParserPos - 1).c_str());
			double rotation = atof(position.substr(yParserPos + 1, rParserPos - yParserPos - 1).c_str());

			client.getShipData().setPositionX(positionX);
			client.getShipData().setPositionY(positionY);
			client.getShipData().setRotation(rotation);
			break;
	
		}

		case pBullet: // RETRIEVING INFORMATION FROM CLIENT ABOUT CREATED BULLET
		{
			std::string bulletInformation;
			if (!communicator->getMessage(client.getSocket(), bulletInformation)) return false;

			bulletInformation = std::to_string(client.getId()) + "I" + bulletInformation;

			for (int i = 0; i < ClientsArray.size(); i++) // SENDING TO OTHER PLAYERS
			{
				if (ClientsArray[i]->getSocket() == client.getSocket()) continue;
				if (!communicator->sendBulletInformation(ClientsArray[i]->getSocket(), bulletInformation))
				{
					std::cout << "Failed to send bulletInformation from client: " << i << std::endl;
				}
			}
			break;
		}

		default:
		{
			std::cout << "Unrecognized packet: " << packetType << std::endl;
			return false;
			break;
		}
		
		
	}
	return true;
}
void ServerData::SendDataToClient(Client & client) // SENDING THREAD
{
	int failedPackets = 0;
	while (client.isOnline() && failedPackets < 1000)
	{
		if (isClientsArrayChanging() == false)
		{
			for (int i = 0; i < client.getClientsArray().size(); i++)
			{

				if (client.getClientsArray()[i]->getId() != client.getId())
				{
					std::string position = std::to_string(client.getClientsArray()[i]->getId()) + "X"
						+ std::to_string(client.getClientsArray()[i]->getShipData().getPositionX())
						+ "Y" + std::to_string(client.getClientsArray()[i]->getShipData().getPositionY())
						+ "R" + std::to_string(client.getClientsArray()[i]->getShipData().getRotation());

					if (!serverPtr->communicator->sendPlayersPosition(client.getSocket(), position))
					{
						failedPackets++;
					}
				}

			}
			Sleep(16);
		}
	}
}
void ServerData::GetDataFromClient(Client & client) // RECEIVING THREAD
{
	Packet packetType;
	int failedPackets = 0; // if there is a problem with client, he ll be disconnected
	while (client.isOnline() && failedPackets < 1000)
	{
		if (!serverPtr->communicator->getPacketType(client.getSocket(), packetType))
		{
			std::cout << "Failed to get packet from: " << client.getId() << std::endl;
			failedPackets++;
		}
		if (!serverPtr->processPacket(client, packetType))
		{
			std::cout << "Failed to process packet from: " << client.getId() << std::endl;
			failedPackets++;
		}
	}

	setClientsArrayIsChanging(true);

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

	//for (int i = 0; i < ClientsArray.size(); i++)
	//{
	//	std::cout << "Klient: " << ClientsArray[i]->getNickname() << std::endl;
	//	for (int j = 0; j < ClientsArray[i]->getClientsArray().size(); j++)
	//	{
	//		std::cout << ClientsArray[i]->getClientsArray()[j]->getNickname() << std::endl;
	//	}
	//}

	for (int i = 0; i < ClientsArray.size(); i++)
	{
		int innerIndex = 0;
		serverPtr->communicator->sendPlayerLeftAlert(ClientsArray[i]->getSocket(), std::to_string(client.getId()));
		for (int j = 0; j < ClientsArray[i]->getClientsArray().size(); j++)
		{
			if (ClientsArray[i]->getClientsArray()[j]->getId() == client.getId())
			{
				innerIndex = j;
			}
		}
		ClientsArray[i]->getClientsArray().erase(ClientsArray[i]->getClientsArray().begin() + innerIndex);
	}

	setClientsArrayIsChanging(false);
}
