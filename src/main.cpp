#include <iostream>
#include "ChatRoomServer.h"
#include "ChatRoomClient.h"

int main()
{
    ChatRoomClient client("users.txt");
    std::cout << client.login("Pete","test") << std::endl;

    return 0;
}