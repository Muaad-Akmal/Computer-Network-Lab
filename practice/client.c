#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <signal.h>

#define PORT 4545
#define NAME_SIZE 30
#define BUFFER_SIZE 100
#define MESSAGE_SIZE 130

int sockfd;
struct sockaddr_in serverAddr;
char name[NAME_SIZE];
volatile sig_atomic_t flag=0;

void error(char *msg){
  perror(msg);
  exit(1);
}

void cntrlC(int sig){
  flag=1;
}

void * readMsg(){

    char msg[MESSAGE_SIZE];

    while(1){
      bzero(msg, MESSAGE_SIZE);
      fflush(stdout);

      int n=recv(sockfd, msg, MESSAGE_SIZE, 0);
      if(n<0)
        error("[-] Error in receiving \n");
      
      msg[strlen(msg)]='\0';

        if((strstr(msg, "SHUTTING DOWN"))!=NULL){
          printf("SERVER SHUTTING DOWN \n");
          flag=1;
          break;
        }

      fflush(stdout);
      printf("%s \n", msg);
    }
}

void * sendMsg(){

  char buffer[BUFFER_SIZE];
  char msg[MESSAGE_SIZE];

  while(1){
     bzero(buffer, BUFFER_SIZE);
     printf("\r%s : " , name);
     fflush(stdout);
     fflush(stdin);
     fgets(buffer, BUFFER_SIZE, stdin);
     buffer[strlen(buffer)-1]='\0';
    //  sprintf(msg,"\r%s : %s", name, buffer);
    sprintf(msg, "%s", buffer);

    if((strncmp(buffer, "/bye", 4))==0){
      printf("[-] Exiting ip: %s port :%d", inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port));
      send(sockfd, msg, strlen(msg), 0);
      break;
    } else{
      send(sockfd, msg, strlen(msg), 0);
      bzero(msg, MESSAGE_SIZE);
    }
  }
    flag=1;
}


int main(){

  sockfd=socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd<0)
    error("[-] Error in socket creation\n");



  //this signal handler is for the client to exit gracefully , when the client presses ctrl+c
  //cntrlc is a software interrupt
  signal(SIGINT, cntrlC);

  printf("Enter your name: ");
  fgets(name, NAME_SIZE, stdin);
  name[strlen(name)-1]='\0';

  memset(&serverAddr, '\0', sizeof(serverAddr));
  serverAddr.sin_family=AF_INET;
  serverAddr.sin_port=htons(PORT);
  serverAddr.sin_addr.s_addr=inet_addr("127.0.0.1");

  if(connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr))<0)
    error("[-] Error in connection\n");

  send(sockfd, name, strlen(name), 0);
  printf("======WELCOME TO THE CHATROOM======\n");

  pthread_t read_t, write_t;

  pthread_create(&write_t, NULL,(void *) sendMsg, NULL);
  pthread_create(&read_t, NULL, (void *) readMsg, NULL);

  while(1){
    if(flag){
      printf("[-] Exiting ip: %s port :%d", inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port));
      break;
    }
  }
  close(sockfd);

  return 0;
}
