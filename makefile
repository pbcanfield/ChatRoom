all:
	make ClientMain
	make ServerMain

ClientMain: ChatRoomClient.o ClientMain.o
	g++ -g -std=c++11 develop/ChatRoomClient.o develop/ClientMain.o -o Client.exe -lws2_32

ServerMain: ChatRoomServer.o ServerMain.o
	g++ -g -std=c++11 develop/ChatRoomServer.o develop/ServerMain.o -o Server.exe -lws2_32

ChatRoomServer.o: src/ChatRoomServer.cpp src/ChatRoomServer.h src/User.h
	g++ -g -std=c++11 -c src/ChatRoomServer.cpp -o develop/ChatRoomServer.o 

ChatRoomClient.o: src/ChatRoomClient.cpp src/ChatRoomClient.h
	g++ -g -std=c++11 -c src/ChatRoomClient.cpp -o develop/ChatRoomClient.o 

ClientMain.o: src/ClientMain.cpp
	g++ -g -std=c++11 -c src/ClientMain.cpp -o develop/ClientMain.o 

ServerMain.o: src/ServerMain.cpp
	g++ -g -std=c++11 -c src/ServerMain.cpp -o develop/ServerMain.o 


clean:
	rm develop/*.o *.exe