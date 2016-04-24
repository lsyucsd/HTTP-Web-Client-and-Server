CC=gcc
CFLAGS=-ggdb 

all: client http-server

client: TCPEchoClient4.c DieWithMessage.c
	$(CC) $(CFLAGS) -pthread -o client TCPEchoClient4.c DieWithMessage.c

http-server: http-server.c DieWithMessage.c TCPServerUtility.c AddressUtility.c
	$(CC) $(CFLAGS) -pthread -o http-server http-server.c DieWithMessage.c TCPServerUtility.c AddressUtility.c

clean:
	rm -rf client http-server client.dSYM http-server.dSYM