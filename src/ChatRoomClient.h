#ifndef PORT
#define HOST_IP "127.0.0.1"
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

class ChatRoomClient {
    public:
        ChatRoomClient(std::string);
        ~ChatRoomClient();

        //Parse a command and execute it.
        void parseCommand(std::string);

        //Writes a new user to a file with the given filename.
        void createNewUser(User);

        //Sends a message to the server if logged in.
        void sendMessage(std::string);

        //Checks if there is a username and password combo that matches the given info.
        void login(std::string, std::string);
        void logout();
    private:
        WSADATA * wsaData;
        bool initialized;
        addrinfo * result;

        SOCKET connectSocket;
void sendData(std::string);
        
        std::string awaitResponse();

        static void parseSpace(std::string, std::string*, std::string*);

        std::string userFileName;
        std::vector<User> users;

        std::string loggedInUser;
        bool loggedIn;
};

#endif