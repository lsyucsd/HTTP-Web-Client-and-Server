#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Practical.h"

#include <sys/timeb.h>

int main(int argc, char *argv[]) {

  if (argc < 3 || argc > 4) // Test for correct number of arguments
    DieWithUserMessage("Parameter(s)",
        "<Server Address> <Echo Word> [<Server Port>]");

  char *servIP = argv[2];     // First arg: server IP address (dotted quad)
  int N = atoi(argv[1]); // Second arg: string to echo

  // Third arg (optional): server port (numeric).  7 is well-known echo port
  in_port_t servPort = (argc == 4) ? atoi(argv[3]) : 7;

  int i=0;
  for(i=0;i<N;i++){
  // Create a reliable, stream socket using TCP
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0)
      DieWithSystemMessage("socket() failed");

    // Construct the server address structure
    struct sockaddr_in servAddr;            // Server address
    memset(&servAddr, 0, sizeof(servAddr)); // Zero out structure
    servAddr.sin_family = AF_INET;          // IPv4 address family
    // Convert address
    int rtnVal = inet_pton(AF_INET, servIP, &servAddr.sin_addr.s_addr);
    if (rtnVal == 0)
      DieWithUserMessage("inet_pton() failed", "invalid address string");
    else if (rtnVal < 0)
      DieWithSystemMessage("inet_pton() failed");
    servAddr.sin_port = htons(servPort);    // Server port

    // Establish the connection to the echo server
    if (connect(sock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
      DieWithSystemMessage("connect() failed");

    struct timeb ts1, ts2;
    time_t t_sec, t_ms, ti;

     //size_t echoStringLen = strlen(echoString); // Determine input length
    int sendLength = strlen("delay,90.0,20.0");
    char *sendString = malloc(sendLength);
    strcpy(sendString, "delay,90.0,20.0");

    // Send the string to the server
      ftime(&ts1);
    ssize_t numBytes = send(sock, sendString, sendLength, 0);
    

    if (numBytes < 0)
      DieWithSystemMessage("send() failed");
    else if (numBytes != sendLength)
      DieWithUserMessage("send()", "sent unexpected number of bytes");

  //Receive the date or time from the server
  //Songyang Li
  ssize_t numBytesRec = 0;
  while(true){
    char buffer[BUFSIZE];
    numBytesRec = recv(sock, buffer, BUFSIZE - 1, 0);
    if(numBytesRec < 0)
      DieWithSystemMessage("recv() failed");
    else if(numBytesRec==0)
      break;
    buffer[numBytesRec] = '\0';
    fputs(buffer, stdout);
  }
    ftime(&ts2);
    t_sec=ts2.time-ts1.time;
    t_ms=ts2.millitm-ts1.millitm;
    ti=t_sec*1000+t_ms-1000;
    printf("%d\n", ti);

  
  fputc('\n', stdout); // Print a final linefeed

  close(sock);
}
  exit(0);
}