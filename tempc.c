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

  if(argc<3)
    error("ERROR, no port provided");

  int sockfd;
  int port = atoi(argv[2]);

  sockfd= socket(AF_INET, SOCK_STREAM, 0);

  if(sockfd<0)
    error("ERROR opening socket");

  struct sockaddr_in serveraddr , clientaddr;
  serveraddr.sin_family=AF_INET;
  serveraddr.sin_port=htons(port);
  struct hostent *server= gethostbyname(argv[1]);
  bcopy((char *)server->h_addr,(char *) &serveraddr.sin_addr.s_addr,server->h_length);
  // inet_pton(AF_INET, argv[1], &serveraddr.sin_addr);



  char buffer[256];
  while (1)
  {
    socklen_t serverlen= sizeof(serveraddr);
    bzero(buffer, 256);
    printf("Client: ");
    fgets(buffer, 255, stdin);
    int readStatus= sendto(sockfd, buffer, strlen(buffer),0, (struct sockaddr *) &serveraddr, serverlen);

    if(readStatus<0)
      error("ERROR reading from socket");


    bzero(buffer, 256);
    int writeStatus= recvfrom(sockfd, buffer, 255,0, NULL,NULL);
    printf("Server: %s", buffer);
  }
  
  close(sockfd);

return 0;
}