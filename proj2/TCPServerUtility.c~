#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include "Practical.h"
#include <stdio.h>
#include <stdlib.h>
//Songyang Li
//#include <time.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

static const int MAXPENDING = 5; // Maximum outstanding connection requests

int SetupTCPServerSocket(const char *service) {
  // Construct the server address structure
  struct addrinfo addrCriteria;                   // Criteria for address match
  memset(&addrCriteria, 0, sizeof(addrCriteria)); // Zero out structure
  addrCriteria.ai_family = AF_UNSPEC;             // Any address family
  addrCriteria.ai_flags = AI_PASSIVE;             // Accept on any address/port
  addrCriteria.ai_socktype = SOCK_STREAM;         // Only stream sockets
  addrCriteria.ai_protocol = IPPROTO_TCP;         // Only TCP protocol

  struct addrinfo *servAddr; // List of server addresses
  int rtnVal = getaddrinfo(NULL, service, &addrCriteria, &servAddr);
  if (rtnVal != 0)
    DieWithUserMessage("getaddrinfo() failed", gai_strerror(rtnVal));

  int servSock = -1;
  struct addrinfo *addr = servAddr;
  for (addr = servAddr; addr != NULL; addr = addr->ai_next) {
    // Create a TCP socket
    servSock = socket(addr->ai_family, addr->ai_socktype,
        addr->ai_protocol);
    if (servSock < 0)
      continue;       // Socket creation failed; try next address

    // Bind to the local address and set socket to listen
    if ((bind(servSock, addr->ai_addr, addr->ai_addrlen) == 0) &&
        (listen(servSock, MAXPENDING) == 0)) {
      // Print local address of socket
      struct sockaddr_storage localAddr;
      socklen_t addrSize = sizeof(localAddr);
      if (getsockname(servSock, (struct sockaddr *) &localAddr, &addrSize) < 0)
        DieWithSystemMessage("getsockname() failed");
      fputs("Binding to ", stdout);
      PrintSocketAddress((struct sockaddr *) &localAddr, stdout);
      fputc('\n', stdout);
      break;       // Bind and listen successful
    }

    close(servSock);  // Close and try again
    servSock = -1;
  }

  // Free address list allocated by getaddrinfo()
  freeaddrinfo(servAddr);

  return servSock;
}

int AcceptTCPConnection(int servSock) {
  struct sockaddr_storage clntAddr; // Client address
  // Set length of client address structure (in-out parameter)
  socklen_t clntAddrLen = sizeof(clntAddr);

  // Wait for a client to connect
  int clntSock = accept(servSock, (struct sockaddr *) &clntAddr, &clntAddrLen);
  if (clntSock < 0)
    DieWithSystemMessage("accept() failed");

  // clntSock is connected to a client!

  fputs("Handling client ", stdout);
  PrintSocketAddress((struct sockaddr *) &clntAddr, stdout);
  fputc('\n', stdout);

  return clntSock;
}

//Songyang Li
void HandleTCPClient(int clntSocket, char *path){
  //First, receive the message from client, stored in total
	//printf("%s\n", path);
  int lenofpath = strlen(path);
  char *pa = malloc(lenofpath+1);
  memcpy(pa, path, lenofpath);
  pa[lenofpath] = '\0';
  char buffer[BUFSIZE];
  char total[1024] = "";
  ssize_t numBytes = recv(clntSocket, buffer, BUFSIZE - 1, 0);
  if(numBytes < 0){        
    if (errno == EWOULDBLOCK) { // a timeout occured
      close(clntSocket);
      return;
    } else {
      DieWithSystemMessage("recv() failed");
    }
  }
  buffer[numBytes] = '\0';
	//printf("%s\n", buffer);
  strcat(total, buffer);
  //char* curr = strstr(total, "\r\n\r\n");
  while(numBytes > 0){
	//printf("called\n");
    char* curr = strstr(total, "\r\n\r\n");
    if(curr != NULL) break;
    numBytes = recv(clntSocket, buffer, BUFSIZE - 1, 0);
    if(numBytes < 0){         
      if (errno == EWOULDBLOCK) { // a timeout occured
        close(clntSocket);
        return;
      } else {
        DieWithSystemMessage("recv() failed");
      }
    }
    buffer[numBytes] = '\0';
	//printf("%s\n", buffer);
    strcat(total, buffer);
  }
  //*****************************************************
  // Deal with the error with 400
  //*****************************************************
  int status = 200;
  // GET
  int lenofget = strlen("GET");
  char *getTmp = malloc(lenofget+1);
  memcpy(getTmp, total, lenofget);
  getTmp[lenofget] = '\0';
  if(strcmp(getTmp, "GET")!=0){
    status = 400;
  }
  // Get the path of the file
  char *a1 = strstr(total, "/");
  char *a2 = strstr(a1, " ");
	//printf("a1: %s\n", a1);
	//printf("a2: %s\n", a2);
  int lenoffile = strlen(a1) - strlen(a2);
  if(lenoffile == strlen("/")){
    strcat(pa, "/");
    strcat(pa, "index.html");
  }else{
    char *file = malloc(lenoffile+1);
    memcpy(file, a1, lenoffile);
    file[lenoffile] = '\0';
    strcat(pa, file);                                               //get the total path of the file
  }
  // HTTP/1.1
  while(a2[0] == ' '){
	a2++;
	//printf("called\n");
  }
  int lenofhttp = strlen("HTTP/1.1");
  char *protocol = malloc(lenofhttp+1);
  memcpy(protocol, a2, lenofhttp);
  protocol[lenofhttp] = '\0';
  if(strcmp(protocol, "HTTP/1.1")!=0){
	//printf("HTTP: called\n");
    status = 400;
  }
  // Host: 
  char *nextLine = strstr(total, "\r\n");
  nextLine = nextLine + strlen("\r\n");
  int lenofhost = strlen("Host:");
  char *host = malloc(lenofhost+1);
  memcpy(host, nextLine, lenofhost);
  host[lenofhost] = '\0';
  if(strcmp(host, "Host:")!=0) status = 400;
  // get the Host (of no use)                                                  //get the string of host
  // host = host + lenofhost;
  // while(host[0] == " ") host++;
  char *nextLine2 = strstr(nextLine, "\r\n");
  // int lenofhvalue = strlen(host) - strlen(nextLine2);
  // char *hvalue = malloc(lenofhvalue+1);
  // memcpy(hvalue, host, lenofhvalue);
  // hvalue[lenofhvalue] = '\0';
  // User-Agent: 
  nextLine2 = nextLine2 + strlen("\r\n");
  int lenofagent = strlen("User-Agent:");
  char *agent = malloc(lenofagent+1);
  memcpy(agent, nextLine2, lenofagent);
  agent[lenofagent] = '\0';
  if(strcmp(agent, "User-Agent:")!=0) status = 400;
  //******************************************************
  // Deal with the error of 403 and 404
  //******************************************************
  struct stat fileStat;
  stat(pa, &fileStat);
	//printf("%s\n", pa);
  if(access(pa, F_OK)==-1 && status!=400) status = 404;
  else if((fileStat.st_mode & S_IROTH) != 4 && status!=400) status = 403;
  else if(status==400) status = 400;
  else status = 200;
  //******************************************************
  // Construct the header and send the file (only when status = 200)
  //******************************************************
	//printf("%d\n", status);
  if(status == 200){
	//printf("called200\n");
    int totalLength = strlen("HTTP/1.1 200 OK")+strlen("\r\n")+strlen("Server: Apache/2")+strlen("\r\n")+strlen("Content-Length: ")+strlen("\r\n")+
      strlen("Content-Type: ")+strlen("\r\n")+strlen("Connection: close")+strlen("\r\n\r\n");
    //get the Content-Length
    FILE *fp = fopen(pa, "r");
    fseek(fp, 0L, SEEK_END);
    int size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    fclose(fp);
    char str[15];
    sprintf(str, "%d", size); //int to char
	
    totalLength = totalLength + strlen(str);
    //get the Content-Type
    char *type = strstr(pa, ".");
    type++;
    if(strcmp(type, "html")==0){
      totalLength = totalLength+strlen("text/html");
      char *sendString = malloc(totalLength);
      strcpy(sendString, "HTTP/1.1 200 OK");
      strcat(sendString, "\r\n");
      strcat(sendString, "Server: Apache/2");
      strcat(sendString, "\r\n");
      strcat(sendString, "Content-Length: ");
      strcat(sendString, str);
      strcat(sendString, "\r\n");
      strcat(sendString, "Content-Type: ");
      strcat(sendString, "text/html");
      strcat(sendString, "\r\n");
      strcat(sendString, "Connection: close");
      strcat(sendString, "\r\n\r\n");
      //sendString[totalLength-1] = '\0';
      ssize_t numBytes = send(clntSocket, sendString, totalLength, 0);
      if (numBytes < 0)
        DieWithSystemMessage("send() failed");
      else if (numBytes != totalLength)
        DieWithUserMessage("send()", "sent unexpected number of bytes");
    }
    else if(strcmp(type, "jpg")==0){
      totalLength = totalLength+strlen("image/jpeg");
      char *sendString = malloc(totalLength);
      strcpy(sendString, "HTTP/1.1 200 OK");
      strcat(sendString, "\r\n");
      strcat(sendString, "Server: Apache/2");
      strcat(sendString, "\r\n");
      strcat(sendString, "Content-Length: ");
      strcat(sendString, str);
      strcat(sendString, "\r\n");
      strcat(sendString, "Content-Type: ");
      strcat(sendString, "image/jpeg");
      strcat(sendString, "\r\n");
      strcat(sendString, "Connection: close");
      strcat(sendString, "\r\n\r\n");
      //sendString[totalLength-1] = '\0';
      ssize_t numBytes = send(clntSocket, sendString, totalLength, 0);
      if (numBytes < 0)
        DieWithSystemMessage("send() failed");
      else if (numBytes != totalLength)
        DieWithUserMessage("send()", "sent unexpected number of bytes");
    }
    else{       //content type is png
      totalLength = totalLength+strlen("image/png");
      char *sendString = malloc(totalLength);
      strcpy(sendString, "HTTP/1.1 200 OK");
      strcat(sendString, "\r\n");
      strcat(sendString, "Server: Apache/2");
      strcat(sendString, "\r\n");
      strcat(sendString, "Content-Length: ");
      strcat(sendString, str);
      strcat(sendString, "\r\n");
      strcat(sendString, "Content-Type: ");
      strcat(sendString, "image/png");
      strcat(sendString, "\r\n");
      strcat(sendString, "Connection: close");
      strcat(sendString, "\r\n\r\n");
      //sendString[totalLength-1] = '\0';
      ssize_t numBytes = send(clntSocket, sendString, totalLength, 0);
      if (numBytes < 0)
        DieWithSystemMessage("send() failed");
      else if (numBytes != totalLength)
        DieWithUserMessage("send()", "sent unexpected number of bytes");
    }
    //the response body, send the file
    int fd = open(pa, O_RDONLY);
	//struct stat stat_buf;
	//fstat(fd, &stat_buf);
    off_t offset = 0;
    int numBytesSend = sendfile(clntSocket, fd, &offset, size);
    if (numBytesSend < 0)
      DieWithSystemMessage("sendfile() failed");
    else if (numBytesSend != size)
      DieWithUserMessage("sendfile()", "sent unexpected number of bytes");
  }else if(status == 400){
    int totalLength = strlen("HTTP/1.1 400 Bad Request")+strlen("\r\n")+strlen("Server: Apache/2")+strlen("\r\n")+strlen("Connection: close")+
      strlen("\r\n\r\n");
    char *sendString = malloc(totalLength);
    strcpy(sendString, "HTTP/1.1 400 Bad Request");
    strcat(sendString, "\r\n");
    strcat(sendString, "Server: Apache/2");
    strcat(sendString, "\r\n");
    strcat(sendString, "Connection: close");
    strcat(sendString, "\r\n\r\n");
    //sendString[totalLength-1] = '\0';
    ssize_t numBytes = send(clntSocket, sendString, totalLength, 0);
    if (numBytes < 0)
      DieWithSystemMessage("send() failed");
    else if (numBytes != totalLength)
      DieWithUserMessage("send()", "sent unexpected number of bytes");
  }else if(status == 403){ // send 403 in the end
    int totalLength = strlen("HTTP/1.1 403 Forbidden")+strlen("\r\n")+strlen("Server: Apache/2")+strlen("\r\n")+strlen("Connection: close")+
      strlen("\r\n\r\n");
    char *sendString = malloc(totalLength);
    strcpy(sendString, "HTTP/1.1 403 Forbidden");
    strcat(sendString, "\r\n");
    strcat(sendString, "Server: Apache/2");
    strcat(sendString, "\r\n");
    strcat(sendString, "Connection: close");
    strcat(sendString, "\r\n\r\n");
    //sendString[totalLength-1] = '\0';
    ssize_t numBytes = send(clntSocket, sendString, totalLength, 0);
    if (numBytes < 0)
      DieWithSystemMessage("send() failed");
    else if (numBytes != totalLength)
      DieWithUserMessage("send()", "sent unexpected number of bytes");
  }else{    // status = 404, send 404 in the end
	//printf("called404\n");
    int totalLength = strlen("HTTP/1.1 404 Not Found")+strlen("\r\n")+strlen("Server: Apache/2")+strlen("\r\n")+strlen("Connection: close")+
      strlen("\r\n\r\n");
    char *sendString = malloc(totalLength);
    strcpy(sendString, "HTTP/1.1 404 Not Found");
    strcat(sendString, "\r\n");
    strcat(sendString, "Server: Apache/2");
    strcat(sendString, "\r\n");
    strcat(sendString, "Connection: close");
    strcat(sendString, "\r\n\r\n");
    //sendString[totalLength-1] = '\0';
    ssize_t numBytes = send(clntSocket, sendString, totalLength, 0);
    if (numBytes < 0)
      DieWithSystemMessage("send() failed");
    else if (numBytes != totalLength)
      DieWithUserMessage("send()", "sent unexpected number of bytes");
  }
  //printf("called\n");
  close(clntSocket);
}
// Close client socket
