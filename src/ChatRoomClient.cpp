#include "ChatRoomClient.h"

ChatRoomClient::ChatRoomClient(std::string userFile) {
    loggedIn = false;
    this->userFileName = userFile;
    
    //Populate the user vector with the users in the given file.
    std::ifstream file(userFile, std::ios::in);
    
    //Check that the file actually was opened.
    if(file.is_open()) {
        //Each user will be stored in two lines, their UID and password.
        std::string UID, pass;

        //Create a new user object and push it to the vector
        while(std::getline(file, UID) && std::getline(file, pass))
            this->users.push_back({UID,pass});

        file.close();
    }
}

ChatRoomClient::~ChatRoomClient() {
    
}

void ChatRoomClient::createNewUser(User user) {
    this->users.push_back(user);

    //Now write to the file.
    std::ofstream file(this->userFileName, std::ios::out | std::ios::app);
    
    if(file.is_open()){
        file << user.UID << std::endl;
        file << user.password << std::endl;

        file.close();
    }
}

bool ChatRoomClient::login(std::string UID, std::string password) {
    bool login = false;
    
    for(User user : this->users) {
        if(user.UID == UID && user.password == password) {
            login = true;
            break;
        }

    }
    this->loggedIn = login;
    return login;
}