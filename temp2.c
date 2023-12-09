#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
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
  struct hostent * server;

  port = atoi(argv[2]);

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if(sockfd<0)
      error("ERROR opening socket");
  

  serveraddr.sin_family = AF_INET;
  serveraddr.sin_port = htons(port);
  
  inet_pton(AF_INET, argv[1], &serveraddr.sin_addr);
  // bcopy((char *) server->h_addr, (char *) &serveraddr.sin_addr.s_addr, server->h_length);
  
  
  char buffer[BUFFER_SIZE];


  while(1){
    server = gethostbyname(argv[1]);
    printf("Host name : %s\n", server->h_name);
    printf("IP : %s",inet_ntoa(* (struct in_addr *)server->h_addr)); 
    bzero(buffer, BUFFER_SIZE);
    printf("Client: ");

    bzero(buffer, BUFFER_SIZE);
    fgets(buffer, BUFFER_SIZE - 1, stdin);
    int n = sendto(sockfd, buffer, strlen(buffer),0, (struct sockaddr *) &serveraddr, sizeof(serveraddr));
    // n = send(sockfd, buffer, strlen(buffer), 0);
    if(n<0)
        error("ERROR writing to socket");

     n = recvfrom(sockfd, buffer, BUFFER_SIZE,0,NULL, NULL);
    printf("server: %s\n", buffer);
    // int n= resv(sockfd, buffer, BUFFER_SIZE, 0);

    if(n<0)
        error("ERROR reading from socket");
    

  }

close(sockfd);


  return 0;
}





// struct sockaddr_in peer_addr;
// socklen_t addr_len = sizeof(peer_addr);

// ret = getpeername(sockfd, (struct sockaddr *)&peer_addr, &addr_len);
// if (ret == -1) {
//     perror("getpeername");
//     return 1;
// }
// printf("Peer address: %s\n", inet_ntoa(peer_addr.sin_addr));