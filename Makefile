build:
	gcc server.c -o server
	gcc client.c -o client

clean:
	rm server client