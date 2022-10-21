#ifndef PORT
#define PORT "4966"
#define BUFF_LEN 256
#define _WIN32_WINNT 0x501

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#include <winsock2.h>
#include <ws2tcpip.h>

#include "User.h"

class ChatRoomServer {
    public:
        ChatRoomServer(std::string);
        ~ChatRoomServer();

        void serverListen();
        

    private:
        void receiveData(SOCKET clientSocket);
        void shutdownSocket(SOCKET clientSocket);

        WSADATA * wsaData;
        bool initialized;
        addrinfo * result;
        
        SOCKET listenSocket;
        
        std::string userFileName;
        std::vector<User> users;

};

#endif