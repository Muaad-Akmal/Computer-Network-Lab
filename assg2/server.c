#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUFFER_SIZE 256
#define PORT 4325


void error(char * msg){
  perror(msg);
  exit(1);
}

void sigchld_handler() {
    pid_t pid;
    int status;
    while((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        printf("Child with pid %d terminated with status %d\n", pid, WEXITSTATUS(status));
    }
}


int main(int argc, char * argv[]){

  int sockfd, clientfd, port;
  struct sockaddr_in serveraddr, clientaddr;
  socklen_t clilen;

  sockfd=socket(AF_INET, SOCK_STREAM , 0);
  if(sockfd<0)
    error("Error opening socket");

  bzero((char *)&serveraddr, sizeof(serveraddr));

  serveraddr.sin_port=htons(PORT);
  serveraddr.sin_family=AF_INET;
  serveraddr.sin_addr.s_addr=inet_addr("127.0.0.1");

  int bindstatus= bind(sockfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr));
  if(bindstatus<0)
    error("Error binding socket");
  
  listen(sockfd, 5);
  clilen=sizeof(clientaddr);

  char buffer[BUFFER_SIZE];

  while (1){ 

    clientfd=accept(sockfd, (struct sockaddr *) &clientaddr, &clilen);
    if(clientfd<0)
      error("Error on accept");
    else
      printf("[+] Connection accepted from client %s %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
    
    int pid= fork();
    if(pid<0)
      error("Error on fork");

    if(pid==0){
      close(sockfd);
      while(1)
     {    bzero(buffer, BUFFER_SIZE);
          int n=recv(clientfd, buffer, BUFFER_SIZE, 0);
				  if(strcmp(buffer, ":exit") == 0){
					    printf("Disconnected from %s:%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
					    break;
          }
          printf("Client: %s", buffer);
          
          fflush(stdout);
          bzero(buffer, BUFFER_SIZE);

     }
        close(clientfd);
    }

   sigchld_handler();
    
}
  
close(sockfd);
return 0;

}