#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]){

  if(argc<3)
    error("Error: No port provided");


  int sockfd, port;
  port= atoi(argv[2]);
  sockfd= socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in client_Addr;

  client_Addr.sin_family= AF_INET;
  client_Addr.sin_port= htons(port);
  client_Addr.sin_addr.s_addr= INADDR_ANY;

  int connect_Status= connect(sockfd, (struct sockaddr* ) &client_Addr, sizeof(client_Addr));

  if(connect_Status<0)
    error("Error: Connection Failed");

  char buffer[256];

  while(1){
    bzero(buffer, 256);
    fgets(buffer, 255, stdin);
    int write_Status= write(sockfd, buffer, strlen(buffer));
    if(write_Status<0)
      error("Error: Writing Failed");

    bzero(buffer, 256);
    int read_Status= read(sockfd, buffer, 255);
    if(read_Status<0)
      error("Error: Reading Failed");
    printf("Server: %s \n", buffer);
    	printf("Client: ");
  }
close(sockfd);
  return 0;
}