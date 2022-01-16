CPP := g++
CFLAG := -lsqlite3 -Wall -lpthread

INPUT_SERVER := server.cpp database.cpp login_interface.cpp relationship.cpp message.cpp
INPUT_CLIENT := client.cpp database.cpp login_interface.cpp relationship.cpp message.cpp
OUTPUT_SERVER := server
OUTPUT_CLIENT := client
OUTPUT_DATABASE := chatroom.db

.PHONY: all clean main

all: db clean server client

clean:
	rm -f $(OUTPUT_SERVER)
	rm -f $(OUTPUT_CLIENT)

server: server.cpp
	$(CPP) $(CFLAG) $(INPUT_SERVER) -o $(OUTPUT_SERVER)

client: client.cpp
	$(CPP) $(CFLAG) $(INPUT_CLIENT) -o $(OUTPUT_CLIENT)

db:
	rm -f $(OUTPUT_DATABASE)
