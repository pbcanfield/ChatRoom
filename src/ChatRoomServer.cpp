#include "ChatRoomServer.h"

ChatRoomServer::ChatRoomServer(std::string userFile) {
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

    ZeroMemory(&hints, sizeof (hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the local address and port to be used by the server
    iResult = getaddrinfo(NULL, PORT, &hints, &(this->result));
    this-> initialized = !iResult; 
    if (!this->initialized) {
        std::cout << "getaddrinfo failed: " <<  iResult << std::endl;
        WSACleanup();
        return;
    }

    //Setup the socket.
    this -> listenSocket = socket(this->result->ai_family, this->result->ai_socktype, this->result->ai_protocol);
    if (this -> listenSocket == INVALID_SOCKET) {
        std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
        freeaddrinfo(this->result);
        WSACleanup();
        return;
    }

    //Bind the socket.
    iResult = bind(this->listenSocket, this->result->ai_addr, (int)this->result->ai_addrlen);
    this-> initialized = !iResult; 
    if (iResult == SOCKET_ERROR) {
        std::cout << "bind failed with error: " << WSAGetLastError() << std::endl;
        freeaddrinfo(this->result);
        closesocket(this->listenSocket);
        WSACleanup();
        return;
    }
}

ChatRoomServer::~ChatRoomServer() {
    freeaddrinfo(this->result);
    closesocket(this->listenSocket);
    WSACleanup();
    delete this -> wsaData;
}

void ChatRoomServer::serverListen() {
    if ( listen( this-> listenSocket, SOMAXCONN ) == SOCKET_ERROR ) {
        std::cout <<  "Listen failed with error: " << WSAGetLastError() << std::endl;
        closesocket(this->listenSocket);
        WSACleanup();
        return;
    }

    //Attempt to establish a connection to one client at first.
    SOCKET clientSocket = INVALID_SOCKET;
    clientSocket = accept(this -> listenSocket, NULL, NULL);


    if(clientSocket == INVALID_SOCKET) {
        std::cout << "accept failed: " << WSAGetLastError() << std::endl;
        closesocket(this->listenSocket);
        WSACleanup();
        return;
    }

    this->receiveData(clientSocket);
}

void ChatRoomServer::receiveData(SOCKET clientSocket) {
    char recBuff[BUFF_LEN];
    int iRecieveResult;

    //Receive data until the connection is terminated.
    do {
        iRecieveResult = recv(clientSocket, recBuff, BUFF_LEN, 0);
        if (iRecieveResult > 0) {
            std::string message = recBuff;
            this->parseCommand(clientSocket, message);   
        }
    } while (iRecieveResult > 0);
}

void ChatRoomServer::sendData(SOCKET clientSocket, std::string message) {
    char msgBuff[BUFF_LEN];

    strcpy(msgBuff, message.c_str());

    int iSendResult = send(clientSocket, msgBuff, BUFF_LEN, 0);
    if (iSendResult == SOCKET_ERROR) {
        std::cout << "send failed: " <<  WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
    }
}

void ChatRoomServer::shutdownSocket(SOCKET clientSocket) {
    closesocket(clientSocket);
    WSACleanup();
}

bool ChatRoomServer::createNewUser(User user) {
    bool completed = false;
    
    //Check that the username and passwords are the correct lengths.
    if ((3 <= user.UID.size() && user.UID.size() <= 32) &&
        (4 <= user.password.size() && user.password.size() <= 8)) {

        //Check that the user doesnt already exist.
        bool userExists = false;
        for(User existingUser : this->users) {
            if(existingUser.UID == user.UID) {
                userExists = true;
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

bool ChatRoomServer::login(std::string UID, std::string password) {
    bool login = false;
    for(User user : this->users) {
        if(user.UID == UID && user.password == password) {
            login = true;
            this->loggedIn = UID;
            break;
        }
    }

    return login;
}

void ChatRoomServer::parseCommand(SOCKET clientSocket, std::string command) {
    std::string code = command.substr(0,3);
    std::string payload = command.substr(3,command.size());
    
    if(code == "lup")
    {
        std::string uid,password;
        this->parseSpace(payload, &uid, &password);

        if(this->login(uid, password)) {
            std::cout << uid + " login." << std::endl;
            this->sendData(clientSocket, "login confirmed");
        }
        else
            this->sendData(clientSocket, "Denied. User name or password incorrect.");
    }
    else if (code == "nup") {
        std::string uid, password;
        this->parseSpace(payload,&uid,&password);

        if(this->createNewUser({uid,password})) {
            std::cout << "New user account created." << std::endl;
            this->sendData(clientSocket, "New user account created. Please login.");
        }
        else
            this->sendData(clientSocket, "Denied. User account already exists.");
    }
    else if (code == "sen") {
        std::string toSend = (this->loggedIn) + ": " + payload;
        std::cout << toSend << std::endl;
        this -> sendData(clientSocket, toSend);
    }
    else if (code == "ext") {
        std::cout << (this->loggedIn) + " logout." << std::endl;
        this -> sendData(clientSocket, (this->loggedIn) + " left.");
        this -> loggedIn = "";
    }


}

void ChatRoomServer::parseSpace(std::string input, std::string * firstToken, std::string * secondToken) {
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