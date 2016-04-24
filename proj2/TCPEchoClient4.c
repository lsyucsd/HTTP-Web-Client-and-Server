#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Practical.h"

int main(int argc, char *argv[]) {

  if (argc < 2 || argc > 3) // Test for correct number of arguments
    DieWithUserMessage("Parameter(s)",
        "<Server Address> <Echo Word> [<Server Port>]");

  char *servIP = argv[1];     // First arg: server IP address (dotted quad)
  //char *echoString = argv[2]; // Second arg: string to echo

  // Third arg (optional): server port (numeric).  7 is well-known echo port
  in_port_t servPort = atoi(argv[2]);

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

  int sendLength = strlen("GET /index.html HTTP/1.1")+strlen("\r\n")+strlen("Host: www.ucsd.edu")+strlen("\r\n")+strlen("User-Agent: lsyucsd")+strlen("\r\n\r\n")+1;
  char *sendString = malloc(sendLength);
  strcpy(sendString, "GET /index.html HTTP/1.1");
  strcat(sendString, "\r\n");
  strcat(sendString, "Host: www.ucsd.edu");
  strcat(sendString, "\r\n");
  strcat(sendString, "User-Agent: lsyucsd");
  strcat(sendString, "\r\n\r\n");

   //size_t echoStringLen = strlen(echoString); // Determine input length

  // Send the string to the server
  //int numBytes = 0;
  //char eachStr[10];
  //int loop = 1;
  //for(loop=1;loop<=sendLength/5+1;loop++){
	//printf("client: called\n");
	//strncpy(eachStr, sendString, 5);
	//eachStr[5] = '\0';
	//numBytes = send(sock, sendString, strlen(eachStr), 0);
	//sendString += 5;
	//sleep(2);
//  }

  ssize_t numBytes = send(sock, sendString, sendLength, 0);
  if (numBytes < 0)
    DieWithSystemMessage("send() failed");
  else if (numBytes != sendLength)
    DieWithUserMessage("send()", "sent unexpected number of bytes");


  // Receive the same string back from the server
  // unsigned int totalBytesRcvd = 0; // Count of total bytes received
  // fputs("Received: ", stdout);     // Setup to print the echoed string
  // while (totalBytesRcvd < echoStringLen) {
  //   char buffer[BUFSIZE]; // I/O buffer
  //   /* Receive up to the buffer size (minus 1 to leave space for
  //    a null terminator) bytes from the sender */
  //   numBytes = recv(sock, buffer, BUFSIZE - 1, 0);
  //   if (numBytes < 0)
  //     DieWithSystemMessage("recv() failed");
  //   else if (numBytes == 0)
  //     DieWithUserMessage("recv()", "connection closed prematurely");
  //   totalBytesRcvd += numBytes; // Keep tally of total bytes
  //   buffer[numBytes] = '\0';    // Terminate the string!
  //   fputs(buffer, stdout);      // Print the echo buffer
  // }

  //Receive the date or time from the server
  //Songyang Li
  ssize_t numBytesRec = 0;
  char buffer[BUFSIZE];
  while(true){
    numBytesRec = recv(sock, buffer, BUFSIZE - 1, 0);
    if(numBytesRec < 0)
      DieWithSystemMessage("recv() failed");
    else if(numBytesRec==0)
      break;
    buffer[numBytesRec] = '\0';
    fputs(buffer, stdout);
  }
  
  fputc('\n', stdout); // Print a final linefeed

  close(sock);
  exit(0);
}
