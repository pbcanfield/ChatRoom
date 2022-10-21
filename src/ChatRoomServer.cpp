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
    int iRecieveResult, ISendResult;

    //Receive data until the connection is terminated.
    do {
        iRecieveResult = recv(clientSocket, recBuff, BUFF_LEN, 0);
        if (iRecieveResult > 0) {
            std::cout << "Buffer: " << recBuff << std::endl;
        }


    } while (iRecieveResult > 0);
}

void ChatRoomServer::shutdownSocket(SOCKET clientSocket) {
    closesocket(clientSocket);
    WSACleanup();
}