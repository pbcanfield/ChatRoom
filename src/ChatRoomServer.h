#ifndef PORT
#define PORT "4966"
//The maximum message size is 256 (+3 for message type metadata.)
#define BUFF_LEN 259
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
        void receiveData(SOCKET);
        void sendData(SOCKET, std::string);
        void shutdownSocket(SOCKET);
        

        WSADATA * wsaData;
        bool initialized;
        addrinfo * result;
        
        SOCKET listenSocket;
        
        bool createNewUser(User);
        bool login(std::string, std::string);

        std::string loggedIn;
        std::string userFileName;
        std::vector<User> users;

        void parseCommand(SOCKET, std::string);
        static void parseSpace(std::string, std::string*, std::string*);

};

#endif