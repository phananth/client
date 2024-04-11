#
# this makefile will compile and and all source
# found in the ~/develop/MyApp directory.  This represents a sample
# development directory structure and project
# 
# =======================================================
#                  MyApp Example
# =======================================================
# FINAL BINARY Target
./bin/chat-client : ./obj/chat-client.o ./obj/chat-network.o ./obj/chat-ui.o
	cc ./obj/chat-client.o ./obj/chat-network.o ./obj/chat-ui.o -o ./bin/chat-client -lpthread -lncurses
#
# =======================================================
#                     Dependencies
# =======================================================                     
./obj/chat-client.o : ./src/chat-client.c ./inc/chat-client.h
	cc -c ./src/chat-client.c -o ./obj/chat-client.o -lncurses
		
./obj/chat-network.o : ./src/chat-network.c ./inc/chat-network.h
	cc -c ./src/chat-network.c -o ./obj/chat-network.o -lncurses
	
./obj/chat-ui.o : ./src/chat-ui.c ./inc/chat-ui.h
	cc -c ./src/chat-ui.c -o ./obj/chat-ui.o -lncurses
#
# =======================================================
# Other targets
# =======================================================                     
all : ./bin/chat-client

clean:
	rm -f ./bin/*
	rm -f ./obj/*.o
	rm -f ./inc/*.h~
	rm -f ./src/*.c~



