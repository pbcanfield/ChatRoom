#ifndef PORT
#define HOST_IP "127.0.0.1"
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

class ChatRoomClient {
    public:
        ChatRoomClient(std::string);
        ~ChatRoomClient();

        //Parse a command and execute it.
        void parseCommand(std::string);

        //Writes a new user to a file with the given filename.
        bool createNewUser(User);

        //Checks if there is a username and password combo that matches the given info.
        bool login(std::string, std::string);
        void logout() {this->loggedIn = false; }
        bool getState() {return this->loggedIn; }

    private:
        WSADATA * wsaData;
        bool initialized;
        addrinfo * result;

        SOCKET connectSocket;

        void sendData(std::string);

        static void parseSpace(std::string, std::string*, std::string*);

        std::string userFileName;
        std::vector<User> users;

        std::string loggedInUser;
        bool loggedIn;
};

#endif