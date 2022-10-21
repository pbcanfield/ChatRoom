#include <iostream>
#include <string>
#include "ChatRoomServer.h"

int main()
{
    std::cout << "\nMy chat room server. Version One.\n" << std::endl; 

    ChatRoomServer server("users.txt");
    server.serverListen();


    return 0;
}