#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Practical.h"
#include <netdb.h>
#include <errno.h>

int main(int argc, char *argv[]) {
  //**************************************************************************
  //Deal with the command line argument
  //**************************************************************************

  if (argc!=2) // Test for correct number of arguments
    DieWithUserMessage("Parameter(s)",
        "HTTP URL");
  
  char *input = argv[1];          //The input is http://www.ucsd.edu:80/index.html
  char *i1 = strchr(input, ':');
  i1 = i1+3;                      //result: www.ucsd.edu(:80)/index.html
  char *i4 = strchr(i1, ':');     //judge if the input contains port number, result: :80/index.html
  int len3 = strlen(i1);
  char *i5 = strchr(i1, '/');     //result: /index.html
  int len5 = strlen(i5);
  int length = len3 - len5;
  in_port_t servPort = 80;
  int lengthPort=strlen("80");
  char *file = i5;
  //char *server;
  char *num;
  int slength;
  if(i4 == NULL){                 //the input doesn't contain port
    //strncpy(server, i1, length);
    num = "80";
    //server[length] ='\0';
  }else{                          //the input contains port
    i4++;                         //result: 80/index.html
    int len4 = strlen(i4);
    lengthPort = len4-len5;
    char a[lengthPort];
    strncpy(a, i4, lengthPort);
    a[lengthPort] = '\0';
    num = &a;
    strncpy(num, i4, lengthPort);
    num[lengthPort]='\0';
    servPort = atoi(num);
    //int length2 = len3 - len5 - lengthPort - 1;
    //strncpy(server, i1, length2);
    //server[length2] = '\0';
  }
  if(i4==NULL){
    slength=length;
  }else{
    slength = len3 - strlen(i4) - 1;
  }
  char server[slength];
  if(i4==NULL){
    strncpy(server, i1, slength);
    server[slength] = '\0';
  }else{
    strncpy(server, i1, slength);
    server[slength] = '\0';
  }
  //printf("%s\n", file);
  //printf("%d\n", servPort);
  //printf("%s\n", server);
  //**************************************************************************
  //Now, we have the server(www.ucsd.edu), port#(servPort), file(/index.html)
  //**************************************************************************

  //char *servIP = argv[1];     // First arg: server IP address (dotted quad)
  //char *echoString = argv[2]; // Second arg: string to echo
  // Third arg (optional): server port (numeric).  7 is well-known echo port
  //in_port_t servPort = 80;

  //**************************************************************************
  //Create a reliable, stream socket using TCP
  //**************************************************************************
  // Create a reliable, stream socket using TCP
  int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sock < 0)
    DieWithSystemMessage("socket() failed");
  struct hostent *serverIP;           //Get the serverIP based on the name
  serverIP = gethostbyname(server);
  // Construct the server address structure
  struct sockaddr_in servAddr;            // Server address
  memset(&servAddr, 0, sizeof(servAddr)); // Zero out structure
  servAddr.sin_family = AF_INET;          // IPv4 address family
    servAddr.sin_port = htons(servPort);    // Server port
  // Convert address
  memcpy(&servAddr.sin_addr.s_addr, serverIP->h_addr, serverIP->h_length);
  //  int rtnVal = inet_pton(AF_INET, (char *)serverIP->h_addr, (char *)&servAddr.sin_addr.s_addr);     //change to serverIP here
  // if (rtnVal == 0)
  //   DieWithUserMessage("inet_pton() failed", "invalid address string");
  // else if (rtnVal < 0)
  //   DieWithSystemMessage("inet_pton() failed");

  // Establish the connection to the echo server
  // Handle error code 1
  if (connect(sock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0){
    fputc('1', stdout);
    exit(-1);
    //DieWithSystemMessage("connect() failed");
  }
  //**************************************************************************
  //Now, we have built the connection
  //**************************************************************************
  //size_t echoStringLen = strlen(echoString); // Determine input length

  //**************************************************************************
  //Send the information
  //**************************************************************************
  // Send the string to the server

  char a[10];
  sprintf(a, "%d", servPort);

  int totalLength = strlen("GET ")+strlen(file)+strlen(" HTTP/1.1\r\n")+strlen("Host: ")+strlen(server)+lengthPort+
    strlen("\r\n")+strlen("User-Agent: ")+strlen("cse124/lsyucsd")+strlen("\r\n")+strlen("\r\n")+1;
  char *sendString = malloc(totalLength);
  strcpy(sendString, "GET ");
  strcat(sendString, file);
  strcat(sendString, " HTTP/1.1\r\n");
  strcat(sendString, "Host: ");
  strcat(sendString, server);
  strcat(sendString, ":");
  strcat(sendString, a);
  strcat(sendString, "\r\n");
  strcat(sendString, "User-Agent: ");
  strcat(sendString, "cse124/lsyucsd");
  strcat(sendString, "\r\n\r\n");
  //printf("%s\n",sendString);
  ssize_t numBytes = send(sock, sendString, totalLength, 0);
  if (numBytes < 0)
    DieWithSystemMessage("send() failed");
  else if (numBytes != totalLength)
    DieWithUserMessage("send()", "sent unexpected number of bytes");

  //**************************************************************************
  //Receive the information
  //**************************************************************************
  char buffer[BUFSIZE];
  numBytes = recv(sock, buffer, BUFSIZE - 1, 0);
  //printf("%d\n", numBytes);
  if(numBytes==-1){
    fputc('2', stdout);
    exit(-1);
  }
  //fputs(buffer, stdout); 
  char *content = strstr(buffer, "\r\n\r\n");
  while(content == NULL){
    //printf("called\n");
    numBytes = recv(sock, buffer, BUFSIZE - 1, 0);
    if(numBytes==-1){
      fputc('2', stdout);
      exit(-1);
    }
    char *content = strstr(buffer, "\r\n\r\n");
    //fputs(buffer, stdout);
  }
  content += 4;

  //**************************** Get the status line ***************************
  // Handle error code 3
  char *curLine = buffer;
  char *nextLine = strstr(curLine, "\r\n");
  int curLineLen = strlen(curLine) - strlen(nextLine);
  char *temp = (char *)malloc(curLineLen+1);
  memcpy(temp, curLine, curLineLen);
  temp[curLineLen] = '\0';
  if(strcmp(temp, "HTTP/1.1 200 OK")!=0 && strcmp(temp, "HTTP/1.1 400 Bad Request")!=0 && strcmp(temp, "HTTP/1.1 403 Forbidden")!=0
    && strcmp(temp, "HTTP/1.1 404 Not Found")!=0){
    fputc('3', stdout);
    exit(-1);
  }

  char *type = strchr(i5, '.');
  type++;

  //**************************** Get the Server ***************************
  char *ser = strcasestr(buffer, "Server:");
  ser = ser + strlen("Server:");
  if(ser[0] == ':'){
    fputc('4', stdout);
    exit(-1);
  }
  while(ser[0] == ' '){
    ser++;
    if(ser[0] == ' '){
      fputc('4', stdout);
      exit(-1);
    }
  }
  
  //**************************** Get the Content Type ***************************
  // Handle error code 7
  char *ct = strcasestr(buffer, "Content-type:");
  ct = ct + strlen("Content-type:");
  if(ct[0] == ':'){
    fputc('4', stdout);
    exit(-1);
  }
  while(ct[0] == ' '){
    ct++;
    if(ct[0] == ' '){
      fputc('4', stdout);
      exit(-1);
    }
  }
  char *nt = strstr(ct, "\r\n");
  int typeLen = strlen(ct)-strlen(nt);
  char *typeTemp = malloc(typeLen+1);
  memcpy(typeTemp, ct, typeLen);
  typeTemp[typeLen]='\0';
  //printf("%s\n", typeTemp);

  //**************************** Get the Content Length ***************************
  // Handle error code 6
  char *cl = strcasestr(buffer, "Content-Length:");
  cl = cl + strlen("Content-Length:");
  if(cl[0] == ':'){
    fputc('4', stdout);
    exit(-1);
  }
  while(cl[0] == ' '){
    cl++;
    if(cl[0]==':'){
      fputc('4', stdout);
      exit(-1);
    }
  }
  char *nl = strstr(cl, "\r\n");
  int lengthLen = strlen(cl)-strlen(nl);
  char *lengthTemp = malloc(lengthLen+1);
  memcpy(lengthTemp, cl, lengthLen);
  lengthTemp[lengthLen]='\0';
  int contentLength = atoi(lengthTemp);
  //printf("%s\n", lengthTemp);
  
  //time-out
  struct timeval tv;
  tv.tv_sec = 3;  /* 3 Second Timeout */
  tv.tv_usec = 0;  // Not init'ing this can cause strange errors
  setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval));

  int totalBytesRcvd = 0;
  if(strcmp(type, "html")==0){
    if(strcmp(typeTemp, "text/html")!=0){
      fputc('7', stdout);
      exit(-1);
    }
    FILE *hfile = fopen("res.html", "w+");
    fputs(content, hfile);
    totalBytesRcvd += strlen(content);
    numBytes = recv(sock, buffer, BUFSIZE - 1, 0);
    totalBytesRcvd += numBytes;
    while(numBytes > 0 && errno!=EWOULDBLOCK){
      fputs(buffer, hfile);
      numBytes = recv(sock, buffer, BUFSIZE - 1, 0);
      totalBytesRcvd += numBytes;
    }
    if(errno==EWOULDBLOCK) totalBytesRcvd++;
    if(contentLength!=0 && totalBytesRcvd==0){
      fputc('5', stdout);
      exit(-1);
    }
    if(totalBytesRcvd!=contentLength){
      fputc('6', stdout);
      exit(-1);
    }
    fclose(hfile);
  }else if(strcmp(type, "png")==0){
    if(strcmp(typeTemp, "image/png")!=0){
      fputc('7', stdout);
      exit(-1);
    }
    FILE *ffile = fopen("res.png", "wb+");
    int lheader = numBytes-strlen(buffer);
    lheader += strlen(content);
    totalBytesRcvd += lheader;
    fwrite(content, sizeof(char), lheader, ffile);
    numBytes = recv(sock, buffer, BUFSIZE - 1, 0);
    totalBytesRcvd += numBytes;
    while(numBytes > 0){
      fwrite(buffer, sizeof(char), numBytes, ffile);
      numBytes = recv(sock, buffer, BUFSIZE - 1, 0);
      totalBytesRcvd += numBytes;
    }
    if(contentLength!=0 && totalBytesRcvd==0){
      fputc('5', stdout);
      exit(-1);
    }
    if(totalBytesRcvd!=contentLength){
      fputc('6', stdout);
      exit(-1);
    }
    fclose(ffile);
  }else{
    if(strcmp(typeTemp, "image/jpeg")!=0){
      fputc('7', stdout);
      exit(-1);
    }
    FILE *ffile = fopen("res.jpg", "wb+");
    int lheader = numBytes-strlen(buffer);
    lheader += strlen(content);
    totalBytesRcvd += lheader;
    fwrite(content, sizeof(char), lheader, ffile);
    numBytes = recv(sock, buffer, BUFSIZE - 1, 0);
    totalBytesRcvd += numBytes;
    while(numBytes > 0){
      fwrite(buffer, sizeof(char), numBytes, ffile);
      numBytes = recv(sock, buffer, BUFSIZE - 1, 0);
      totalBytesRcvd += numBytes;
    }
    if(contentLength!=0 && totalBytesRcvd==0){
      fputc('5', stdout);
      exit(-1);
    }
    if(totalBytesRcvd!=contentLength){
      fputc('6', stdout);
      exit(-1);
    }
    fclose(ffile);
  }

  fputc('0', stdout);
  //fputc('\n', stdout); // Print a final linefeed

  close(sock);
  exit(0);
}