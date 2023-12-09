#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>

#define PORT 4567
#define MAX_CLIENT 10
#define bufferSize 100
#define nameSize 20

typedef struct {
  int sockfd;
  struct sockaddr_in address;
  int id;
  char name[nameSize];
} client_t;

client_t * clients[MAX_CLIENT];
static int uid=0;
static _Atomic unsigned int clientcount=0;

int sockfd;

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;


void enqueue(client_t * cli){
  pthread_mutex_lock(&mut);
  for(int i=0;i<MAX_CLIENT;i++){
    if(clients[i]==NULL){
        clients[i]=cli;
        break;
    }
  }
  pthread_mutex_unlock(&mut);
}

void dequeue(int id){
  pthread_mutex_lock(&mut);
  for(int i=0;i<MAX_CLIENT;i++){
    if(clients[i] && clients[i]->id==id){
          free(clients[i]);
          clients[i]=NULL;
          break;
    }
  }
  pthread_mutex_unlock(&mut);
}

void * handler(void * arg){

  client_t * cli= (client_t *)arg;
  enqueue(cli);
  char name[nameSize];
  recv( cli->sockfd , name, nameSize,0 );
  int leavflag=0;
  char buffer[bufferSize];



  while(!leavflag){
    bzero(buffer, bufferSize);
    recv( cli->sockfd , name, nameSize,0 );
    if(strstr(buffer, "Bye")!=NULL){
      leavflag=1;
      printf("%s is leaving", name);
    }

    printf("%s : %s\n",name, buffer );
    fflush(stdout);
  }
  clientcount--;
  close(cli->sockfd);
  dequeue(cli->id);
  pthread_detach(pthread_self());
  return NULL;
}



int main(){

  sockfd=socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in serveraddr, clientaddr;

  signal(SIGPIPE, SIG_IGN);
  setsockopt(sockfd, SOL_SOCKET, (SO_REUSEADDR) , &(int){1}, 4 );

  serveraddr.sin_family= AF_INET;
  serveraddr.sin_port= htons(PORT);
  serveraddr.sin_addr.s_addr=inet_addr("127.0.0.1");

  socklen_t slen= sizeof(serveraddr);
  int bs= bind(sockfd, (struct sockaddr *) &serveraddr,  slen);

  int ls= listen(sockfd, MAX_CLIENT);

  socklen_t clilen= sizeof(clientaddr);
  
  while(1){

    if(clientcount+1==MAX_CLIENT){
      printf("MAX CLIENTS\n");
      continue;
    }



    int clientsocket= accept(sockfd,  (struct sockaddr *) &clientaddr,  &clilen);

    client_t *cli= (client_t *) malloc(sizeof(client_t));
    cli->address=clientaddr;
    cli->id= uid++;
    cli->sockfd= clientsocket;

    pthread_t th;
    
    pthread_create(&th, NULL, handler, (void *)cli );

  }

  close(sockfd);
  return 0;
}







