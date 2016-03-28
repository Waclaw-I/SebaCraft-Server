#include "ServerData.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <conio.h>
#include <string>

using namespace std;

int main()
{
	ServerData myServer(33355, false);
	while(true) myServer.listenForNewConnection();
}