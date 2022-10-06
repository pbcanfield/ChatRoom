ChatRoom: ChatRoomServer.o ChatRoomClient.o main.o
	g++ -g -std=c++11 develop/ChatRoomServer.o develop/ChatRoomClient.o develop/main.o -o ChatRoom.exe

ChatRoomServer.o: src/ChatRoomServer.cpp src/ChatRoomServer.h src/User.h
	g++ -g -std=c++11 -c src/ChatRoomServer.cpp -o develop/ChatRoomServer.o

ChatRoomClient.o: src/ChatRoomClient.cpp src/ChatRoomClient.h
	g++ -g -std=c++11 -c src/ChatRoomClient.cpp -o develop/ChatRoomClient.o

main.o: src/main.cpp
	g++ -g -std=c++11 -c src/main.cpp -o develop/main.o

clean:
	rm develop/*.o *.exe