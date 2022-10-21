#include <iostream>
#include <string>
#include "ChatRoomServer.h"

int main()
{

    ChatRoomServer server("users.txt");
    server.serverListen();


    return 0;
}