CC=gcc

ifeq ($(OS), Windows_NT)
	# Windows args
	CC_ARGS=-std=c17 -lws2_32
	CLIENT_BIN=client.exe
	SERVER_BIN=server.exe
	RM_CMD=del
	RUN_PREFIX=
else
	# Unix args
	CC_ARGS=-std=c17 -pthread
	CLIENT_BIN=client
	SERVER_BIN=server
	RM_CMD=rm
	RUN_PREFIX=./
endif

build: client server
clean: cleanclient cleanserver
test: testserver testclient

# client
client:
	$(CC) src/client.c -o $(CLIENT_BIN) $(CC_ARGS)

runclient: cleanclient client
	$(RUN_PREFIX)$(CLIENT_BIN)

cleanclient:
	-$(RM_CMD) $(CLIENT_BIN)

# server
server:
	$(CC) src/server.c -o $(SERVER_BIN) $(CC_ARGS)

runserver: cleanserver server
	$(RUN_PREFIX)$(SERVER_BIN)

cleanserver:
	-$(RM_CMD) $(SERVER_BIN)

## testing

testserver:
	$(RUN_PREFIX)test/server.sh

testclient:
	$(RUN_PREFIX)test/client.sh