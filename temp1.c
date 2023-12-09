#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<unistd.h>


#define BUFFER_SIZE 256

void error(const char * message){
  perror(message);
  exit(1);
}


int main(int argc, char * argv[]){

  if(argc<2)
      error("Usage: ./clientTCP hostname port\n");

  int sockfd;
   int port;
  struct sockaddr_in clientaddr, serveraddr;
  

  port = atoi(argv[1]);

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd<0)
      error("ERROR opening socket");
  
  

  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = INADDR_ANY;
  serveraddr.sin_port = htons(port);
  
  int bindstatus= bind(sockfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr));
  if(bindstatus<0)
      error("ERROR binding socket");

  int listenStatus = listen(sockfd, 5);
  if(listenStatus<0)
      error("ERROR listening");
  
  socklen_t  clilen= sizeof(clientaddr);

  int clientsockfd= accept(sockfd, (struct sockaddr *) &clientaddr,&clilen);

  if(clientsockfd<0)
      error("ERROR accepting");


  char buffer[BUFFER_SIZE];
  while(1){

    bzero(buffer, BUFFER_SIZE);

    int n = read(clientsockfd, buffer, BUFFER_SIZE);
    // int n= resv(clientsockfd, buffer, BUFFER_SIZE, 0);

    if(n<0)
        error("ERROR reading from socket");
    
    printf("Client: %s\n", buffer);

    bzero(buffer, BUFFER_SIZE);
    fgets(buffer, BUFFER_SIZE - 1, stdin);
    n = write(clientsockfd, buffer, strlen(buffer));
    // n = send(clientsockfd, buffer, strlen(buffer), 0);
    if(n<0)
        error("ERROR writing to socket");

  }

close(sockfd);
close(clientsockfd);

  return 0;
}


