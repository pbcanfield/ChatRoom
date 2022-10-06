#include <string>
#include <vector>
#include <fstream>
#include "User.h"

class ChatRoomClient {
    public:
        ChatRoomClient(std::string);
        ~ChatRoomClient();

        //Writes a new user to a file with the given filename.
        void createNewUser(User);

        //Checks if there is a username and password combo that matches the given info.
        bool login(std::string, std::string);
        bool logout() {this->loggedIn = false; }
        bool getState() {return this->loggedIn; }

    private:
        std::string userFileName;
        std::vector<User> users;

        bool loggedIn;
};