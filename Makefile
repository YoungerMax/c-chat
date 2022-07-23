# Unix args
ifeq ($(OS), Windows_NT)
	CC=clang
	CC_ARGS=-std=c17 -lws2_32
	CLIENT_BIN=client.exe
	SERVER_BIN=server.exe
	RM_CMD=del
	RUN_PREFIX=
else
	CC=clang
	CC_ARGS=-std=c17
	CLIENT_BIN=client
	SERVER_BIN=server
	RM_CMD=rm
	RUN_PREFIX=./
endif


build: client server
clean: cleanclient cleanserver

# client
client:
	$(CC) client.c -o $(CLIENT_BIN) $(CC_ARGS)

runclient: cleanclient client
	$(RUN_PREFIX)$(CLIENT_BIN)

cleanclient:
	-$(RM_CMD) $(CLIENT_BIN)

# server
server:
	$(CC) server.c -o $(SERVER_BIN) $(CC_ARGS)

runserver: cleanserver server
	$(RUN_PREFIX)$(SERVER_BIN)

cleanserver:
	-$(RM_CMD) $(SERVER_BIN)
