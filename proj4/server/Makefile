CC=gcc
CFLAGS=-ggdb 

all: DelayClient DelayMe

DelayClient: DelayClient.c DieWithMessage.c
	$(CC) $(CFLAGS) -pthread -o DelayClient DelayClient.c DieWithMessage.c

DelayMe: DelayMe.c DieWithMessage.c TCPServerUtility.c AddressUtility.c
	$(CC) $(CFLAGS) -lgsl -pthread -o DelayMe DelayMe.c DieWithMessage.c TCPServerUtility.c AddressUtility.c

clean:
	rm -rf DelayClient DelayMe DelayClient.dSYM DelayMe.dSYM