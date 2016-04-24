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
#include <fcntl.h>
#include <errno.h>

int main(int argc, char *argv[]) {

  if (argc < 3 || argc > 4) // Test for correct number of arguments
    DieWithUserMessage("Parameter(s)",
        "<Request times> <Server address> [<Server Port>]");

  char *servIP = argv[2];     // First arg: server IP address (dotted quad)
  int N = atoi(argv[1]); // Second arg: string to echo

  // Third arg (optional): server port (numeric).  7 is well-known echo port
  in_port_t servPort = (argc == 4) ? atoi(argv[3]) : 7;

  int i=0;
  int array[N];
  time_t start_sec[N];
  time_t start_msec[N];
  for(i=0;i<N;i++){
  // Create a reliable, stream socket using TCP
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0)
      DieWithSystemMessage("socket() failed");
    
    //printf("called\n");
    array[i] = sock;  //use an array to store the socks

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
    if (connect(array[i], (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
      DieWithSystemMessage("connect() failed");

    fcntl(array[i], F_SETFL, O_NONBLOCK);

    struct timeb ts1;
    

     //size_t echoStringLen = strlen(echoString); // Determine input length
    int sendLength = strlen("delay,90.0,20.0");
    char *sendString = malloc(sendLength);
    strcpy(sendString, "delay,90.0,20.0");

    // Send the string to the server
      ftime(&ts1);
      start_sec[i] = ts1.time;
      start_msec[i] = ts1.millitm;
    ssize_t numBytes = send(sock, sendString, sendLength, 0);
    if (numBytes < 0)
      DieWithSystemMessage("send() failed");
    else if (numBytes != sendLength)
      DieWithUserMessage("send()", "sent unexpected number of bytes");

  //Receive the date or time from the server
  //Songyang Li
  }

  int j=0;
  int count = 0;
  while(count < N){
    for(j=0;j<N;j++){
      ssize_t numBytesRec = 0;
      char buffer[BUFSIZE];
      //printf("%d\n", j);
      numBytesRec = recv(array[j], buffer, BUFSIZE - 1, 0);
      //printf("recv: %d\n", numBytesRec);
      // if(numBytesRec<0 || errno == EWOULDBLOCK || numBytesRec == 0){
      //   //printf("called");
      //   continue;
      // }
      //if(numBytesRec < 0)
        //DieWithSystemMessage("recv() failed");
      //else if(numBytesRec==0)
        //break;
      if(numBytesRec > 0){
        count++;
        buffer[numBytesRec] = '\0';
        fputs(buffer, stdout);

        time_t t_sec, t_ms, ti;
        struct timeb ts2;

        ftime(&ts2);
        t_sec=ts2.time - start_sec[j];
        t_ms=ts2.millitm - start_msec[j];
        ti=t_sec*1000+t_ms;
        printf("%d\n", ti);
        fputc('\n', stdout); // Print a final linefeed
        close(array[j]);
      }
    }
  }
  exit(0);
}