#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUFFER_SIZE 256   
#define PORT 4325

void error(const char * message){
  perror(message);
  exit(1);
}

int main(int argc, char * argv[]){

  int clientfd, port;
  struct sockaddr_in clientaddr,serveraddr;

  clientfd=socket(AF_INET,SOCK_STREAM,0);

  serveraddr.sin_family=AF_INET;
  serveraddr.sin_port=htons(PORT);
  serveraddr.sin_addr.s_addr=inet_addr("127.0.0.1");
  
  int connectstatus= connect(clientfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
  if(connectstatus==-1)
    error("Error in connecting to server");

  char buffer[BUFFER_SIZE];

  while(1){
    bzero(buffer,BUFFER_SIZE);
    printf("Client: ");
    fgets(buffer,BUFFER_SIZE,stdin);
  fflush(stdin);

    int n=send(clientfd,buffer,strlen(buffer),0);
   
		if(strcmp(buffer, ":exit") == 0){
			close(clientfd);
			printf("[-]Disconnected from server.\n");
			exit(1);
		}

    bzero(buffer,BUFFER_SIZE);
    // n=recv(clientfd,buffer,BUFFER_SIZE,0);
    // if(n<0)
    //   error("Error in receiving message");
    // printf("Server: %s",buffer);
  }


  return 0;
}