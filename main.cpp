#include "ServerData.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <conio.h>
#include <string>

using namespace std;

int main()
{
	ServerData::Initialize();
	ServerData::WaitForConnection();
}