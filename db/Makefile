CPP := g++
CFLAG := -lsqlite3 -Wall

INPUT_MAIN := main.cpp database.cpp login_interface.cpp relationship.cpp message.cpp
OUTPUT_MAIN := main
OUTPUT_DATABASE := chatroom.db

.PHONY: all clean main

all: clean main

clean:
	rm -f $(OUTPUT_MAIN)
	rm -f $(OUTPUT_DATABASE)

main: main.cpp
	$(CPP) $(CFLAG) $(INPUT_MAIN) -o $(OUTPUT_MAIN)
