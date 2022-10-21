#include <iostream>
#include <string>
#include "ChatRoomClient.h"

int main()
{

    std::cout << "\nMy chat room client. Version One.\n" << std::endl; 
    ChatRoomClient client("users.txt");
    
    std::string input;
    do
    {
        std::getline(std::cin, input);
        client.parseCommand(input);

    }while (input != "done");

    return 0;
}