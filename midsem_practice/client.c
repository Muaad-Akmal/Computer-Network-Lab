#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sockets.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

#define PORT 4567
#define BS 100
#define MS 200
#define NS 20

char name[NS];
volatile sig_atomic_t flag=0;
int sockfd;

struct sockaddr_in serveraddr;

void cntrlC(int sig){
  flag=0;
}

int main(){
  sockfd= socket(AF_INET< SOCK_STREAM,0);

  serveraddr.sin_family= AF_INET;
  serveraddr.sin_addr.s_addr=inet_addr("127.0.0.1");
  serveraddr.sin_port= htons(PORT);

   signal(SIGINT,cntrlC)

    printf("Enter Name: ");
  fgets(name, MAX_NAME, stdin);
  name[strlen(name)-1]='\0';

 if(connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr))<0)
    error("[-] Connection failed \n");

  send(sockfd, name, strlen(name), 0);
  printf("======CHATROOM LIVE======\n");

  pthread_t readth, writeth
  pthread_create(&writeth, NULL, &writemsg, NULL);
  pthread_create(&readth, NULL, &readmsg, NULL);

while(1){
  if(flag){
    printf("EXiting");
    break;
  }

}
  close(sockfd);
  return 0;
}