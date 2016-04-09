#pragma once
#include <Winsock2.h>
#include <string>


class Client
{

private:
	static int allTimeAmount;
	static int actualAmount;
	std::string nickname;
	SOCKET * mySocket;
	int id;

public:

	Client(std::string nickname, SOCKET * socket);
	~Client();

	void setNickname(std::string nickname);
	void setSocket(SOCKET * mySocket);

	std::string getNickname();
	SOCKET & getSocket();
	int getId();
};