CC = gcc
CFLAGS = -Wall -g -pthread -I .
BIN = bin
OBJ = obj

all: $(BIN)/server $(BIN)/client

$(BIN)/server: $(OBJ)/server.o $(OBJ)/common.o | $(BIN)
	$(CC) $(CFLAGS) $(OBJ)/server.o $(OBJ)/common.o -o $(BIN)/server

$(OBJ)/server.o: server.c | $(OBJ)
	$(CC) server.c $(CFLAGS) -c  -o $(OBJ)/server.o

$(BIN)/client: $(OBJ)/client.o $(OBJ)/common.o | $(BIN)
	$(CC) $(CFLAGS) $(OBJ)/client.o $(OBJ)/common.o -o $(BIN)/client

$(OBJ)/client.o: client.c | $(OBJ)
	$(CC) client.c $(CFLAGS) -c -o $(OBJ)/client.o

$(OBJ)/common.o: common.c | $(OBJ)
	$(CC) common.c $(CFLAGS) -c -o $(OBJ)/common.o

$(OBJ): 
	mkdir $(OBJ)

$(BIN):
	mkdir $(BIN)

clean:
	rm -rf $(BIN) $(OBJ)