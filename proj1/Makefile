CC=gcc
CFLAGS=-D_GNU_SOURCE -std=c99

all: http-client server

http-client: http-client.c DieWithMessage.c
	$(CC) $(CFLAGS) -o http-client http-client.c DieWithMessage.c

server: TCPEchoServer4.c DieWithMessage.c TCPServerUtility.c AddressUtility.c
	$(CC) $(CFLAGS) -o server TCPEchoServer4.c DieWithMessage.c TCPServerUtility.c AddressUtility.c

clean:
	rm -rf client server http-client.dSYM server.dSYM