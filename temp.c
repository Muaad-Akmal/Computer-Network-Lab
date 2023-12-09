#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>


//server
void error(const char * message ){
  perror(message);
  exit(1);
}



int main(int argc, char * argv[]){

  if(argc<2)
    error("ERROR, no port provided");

  int sockfd;
  int port = atoi(argv[1]);

  sockfd= socket(AF_INET, SOCK_DGRAM, 0);

  if(sockfd<0)
    error("ERROR opening socket");

  struct sockaddr_in serveraddr , clientaddr;
  serveraddr.sin_family=AF_INET;
  serveraddr.sin_port=htons(port);
  serveraddr.sin_addr.s_addr= INADDR_ANY;

  int bindStatus= bind(sockfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr));

  if(bindStatus<0)
    error("ERROR on binding");


  socklen_t clilen= sizeof(clientaddr);
  // int clientsockfd= accept(sockfd, (struct sockaddr *) &clientaddr, &clilen);

  char buffer[256];
  while (1)
  {
    bzero(buffer, 256);
    int readStatus= recvfrom(sockfd, buffer, 255,0,(struct sockaddr *) &clientaddr, &clilen);

    if(readStatus<0)
      error("ERROR reading from socket");

    printf("Client: %s", buffer);

    bzero(buffer, 256);

    printf("Server: ");
    fgets(buffer, 255, stdin);
    int writeStatus= sendto(sockfd,buffer,strlen(buffer),0,(struct sockaddr *) &clientaddr, clilen);
  }
  
  close(sockfd);

return 0;
}