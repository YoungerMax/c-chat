CC=clang
CLIENT_BIN=client
SERVER_BIN=server

build: client server
clean: cleanclient cleanserver

# client
client:
	$(CC) client.c -o $(CLIENT_BIN)

runclient: cleanclient client
	./$(CLIENT_BIN)

cleanclient:
	-rm $(CLIENT_BIN)

# server
server:
	$(CC) server.c -o $(SERVER_BIN)

runserver: cleanserver server
	./$(SERVER_BIN)

cleanserver:
	-rm $(SERVER_BIN)
