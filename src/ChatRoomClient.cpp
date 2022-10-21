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
            this->login(uid,password);
        } 
        else {
            std::cout << "\"login\" command not formatted correctly." << std::endl;
        }

    } 
    else if (commandToken == "newuser") {
        std::string uid, password;
        
        this->parseSpace(inputToken,&uid,&password);

        if(uid != "" && password != "") 
            this -> createNewUser({uid, password});

    } 
    else if (commandToken == "send") {
        this -> sendMessage(inputToken);
    }
    else if (commandToken == "logout") {
        this->logout();
    }

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
}

std::string ChatRoomClient::awaitResponse() {
    char recBuff[BUFF_LEN];
    
    int iRecieveResult;
    std::string result = "";

    // Receive data until the server closes the connection
    do {
        iRecieveResult = recv(this -> connectSocket, recBuff, BUFF_LEN, 0);
        if (iRecieveResult > 0) {
            result = recBuff;
            break;
        }
        
    } while (iRecieveResult > 0);

    return result;
}

void ChatRoomClient::login(std::string UID, std::string password) {
    //Send the data with the type prefix.
    std::string toSend = "lup" + UID + ' ' + password;
    this -> sendData(toSend);

    std::string response = this->awaitResponse();

    if(response == "login confirmed") 
        this -> loggedIn = true;
    else 
        this -> loggedIn = false;

    std::cout << response << std::endl;
}

void ChatRoomClient::createNewUser(User user) {
    //Send the data with the type prefix.
    std::string toSend = "nup" + user.UID + ' ' + user.password;
    this -> sendData(toSend);

    std::string response = this->awaitResponse();

    if(response == "New user account created. Please login.") 
        this -> loggedIn = true;
    else 
        this -> loggedIn = false;

    std::cout << response << std::endl;
}

void ChatRoomClient::sendMessage(std::string message) {
    if(!this->loggedIn) {
        std::cout << "Denied. Please login first." << std::endl;
        return;
    }
    
    std::string toSend = "sen" + message;
    this->sendData(toSend);
    
    std::string response = this->awaitResponse();
    std::cout << response << std::endl;
}

void ChatRoomClient::logout()
{
    if(!this->loggedIn) {
        std::cout << "Denied. Please login first." << std::endl;
        return;
    }
    
    this -> loggedIn = false;
    std::string toSend = "ext";
    this->sendData(toSend);
    
    std::string response = this->awaitResponse();
    std::cout << response << std::endl;
}