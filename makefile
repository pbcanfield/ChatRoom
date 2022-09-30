ChatRoom: ChatRoomServer.o ChatRoomClient.o main.o
	g++ develop/ChatRoomServer.o develop/ChatRoomClient.o develop/main.o -o ChatRoom.exe

ChatRoomServer.o: src/ChatRoomServer.cpp src/ChatRoomServer.h
	g++ -c src/ChatRoomServer.cpp -o develop/ChatRoomServer.o

ChatRoomClient.o: src/ChatRoomClient.cpp src/ChatRoomClient.h
	g++ -c src/ChatRoomClient.cpp -o develop/ChatRoomClient.o

main.o: src/main.cpp
	g++ -c src/main.cpp -o develop/main.o

clean:
	rm develop/*.o *.exe