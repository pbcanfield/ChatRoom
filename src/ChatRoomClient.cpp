#include "ChatRoomClient.h"

ChatRoomClient::ChatRoomClient(std::string userFile) {
    this->loggedIn = false;
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

    //Initialize the socket.
    this -> wsaData = new WSADATA;
    int iResult = WSAStartup(MAKEWORD(2,2), this -> wsaData);
    
    this -> initialized = !iResult;
    if(!this->initialized)
    {
        std::cout << "Error, WSAStartup failed." << std::endl;
        return;
    }

    //Get address info.
    this->result = NULL;
    addrinfo hints;
    addrinfo * ptr = NULL;

    ZeroMemory(&hints, sizeof (hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the local address and port to be used by the server
    iResult = getaddrinfo(HOST_IP, PORT, &hints, &(this->result));
    this-> initialized = !iResult; 
    if (!this->initialized) {
        std::cout << "getaddrinfo failed: " <<  iResult << std::endl;
        WSACleanup();
        return;
    }
    ptr = this -> result;

    //Setup the socket.
    this -> connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
    if (this -> connectSocket == INVALID_SOCKET) {
        std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
        freeaddrinfo(this->result);
        WSACleanup();
        return;
    }

    //Connect to server.
    iResult = connect( connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        closesocket(connectSocket);
        connectSocket = INVALID_SOCKET;
    }

    freeaddrinfo(this->result);

    if(this->connectSocket == INVALID_SOCKET) {
        std::cout << "unable to connect to the server." << std::endl;
        WSACleanup();
        return;
    }        
}

ChatRoomClient::~ChatRoomClient() {
    
    //Shut down the sending socket.
    freeaddrinfo(this->result);
    closesocket(this->connectSocket);
    WSACleanup();
    delete this -> wsaData;
}

void ChatRoomClient::parseCommand(std::string command) {
    std::string commandToken, inputToken;
    this->parseSpace(command,&commandToken,&inputToken);

    //Now perform the proper actions based on the commands.
    //std::string validCommands[NUM_COMMANDS] = {"login", "newuser", "send", "logout"};
    if (commandToken == "login") {
        //Get the UID and the password to log in.
        std::string uid, password;
        this->parseSpace(inputToken,&uid,&password);

        if(uid != "" && password != "") {
            if(this -> login(uid, password)) {
                std::cout << "login confirmed" << std::endl;
            }
            else {
                std::cout << "Denied. Please login first." << std::endl;
            }
        } 
        else {
            std::cout << "\"login\" command not formatted correctly." << std::endl;
        }

    } 
    else if (commandToken == "newuser") {
        std::string uid, password;
        
        this->parseSpace(inputToken,&uid,&password);
        std::cout << uid << ", " << password << std::endl;

        if(uid != "" && password != "") {
            if(this -> createNewUser({uid, password})) {
                std::cout << "New user account created. Please login." << std::endl;
            }
        } 
        else {
            std::cout << "\"newuser\" command not formatted correctly." << std::endl;
        }

    } 
    else if (commandToken == "send") {
        this -> sendData(inputToken);
    }
    else if (commandToken == "logout") {
        this->logout();
    }

}

bool ChatRoomClient::createNewUser(User user) {
    bool completed = false;
    
    //Check that the username and passwords are the correct lengths.
    if ((3 <= user.UID.size() && user.UID.size() <= 32) &&
        (4 <= user.password.size() && user.password.size() <= 8)) {

        //Check that the user doesnt already exist.
        bool userExists = false;
        for(User existingUser : this->users) {
            if(existingUser.UID == user.UID) {
                userExists = true;
                std::cout << "Denied. User account already exists." << std::endl;
                break;
            }
        }

        if(!userExists) {
            //Now write to the file.
            std::ofstream file(this->userFileName, std::ios::out | std::ios::app);

            if(file.is_open()){
                file << user.UID << std::endl;
                file << user.password << std::endl;

                this->users.push_back(user);

                file.close();
                completed = true;
            }
        }
    }

    return completed;
}

bool ChatRoomClient::login(std::string UID, std::string password) {
    bool login = false;
    for(User user : this->users) {
        if(user.UID == UID && user.password == password) {
            login = true;
            break;
        }
    }

    this->loggedInUser = UID;
    this->loggedIn = login;
    return login;
}


void ChatRoomClient::parseSpace(std::string input, std::string * firstToken, std::string * secondToken) {
    std::size_t firstSpace = input.find(' ');

    if (firstSpace == std::string::npos) {
        *firstToken = input;
        *secondToken = "";
    }
    else {
        std::string first, second;
        *firstToken = input.substr(0,firstSpace);
        *secondToken = input.substr(firstSpace+1,input.size());
    }
    
}

void ChatRoomClient::sendData(std::string message) {
    char msgBuff[BUFF_LEN];
    strcpy(msgBuff, message.c_str());

    int iResult = send(this -> connectSocket, msgBuff, BUFF_LEN, 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed: %d\n", WSAGetLastError());
        closesocket(this -> connectSocket);
        WSACleanup();
    }

    std::cout << "Bytes Sent: " <<  iResult << std::endl;
}