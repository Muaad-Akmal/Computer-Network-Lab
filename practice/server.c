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
#define MAX_CLIENTS 10
#define NAME_SIZE 40
#define BUFFER_SIZE 80

int sockfd;

typedef struct{
  struct sockaddr_in address;
  int id;
  char name[NAME_SIZE];
  int sockfd;
}client_t;

client_t * clients  [MAX_CLIENTS];

static int uid=0;
int _Atomic clientCount=0;
int _Atomic terminator=0;

void error(char *msg){
  perror(msg);
  exit(1);
}

pthread_mutex_t MUTEX =PTHREAD_MUTEX_INITIALIZER;

void pushclient(client_t * cl){
  pthread_mutex_lock(&MUTEX);
  for(int i=0;i<MAX_CLIENTS;i++){
    if(clients[i]==NULL){
        clients[i]=cl;
        break;
    }
  }
  pthread_mutex_unlock(&MUTEX);
}

void popclient(int id){
  pthread_mutex_lock(&MUTEX);
  for(int i=0;i<MAX_CLIENTS;i++){
    if(clients[i] && clients[i]->id==id){
        free(clients[i]);
        clients[i]=NULL;
        break;
    }
  }
  pthread_mutex_unlock(&MUTEX);
}

void sendall(int id,char *s){
  pthread_mutex_lock(&MUTEX);
  for(int i=0;i<MAX_CLIENTS;i++){
    if(clients[i] && clients[i]->id!=id){
        if(write(clients[i]->sockfd, s, strlen(s))<0){
            error("[-] Error in sending \n");
        }
    }
  }
  pthread_mutex_unlock(&MUTEX);
}

void * handler(void * cli){

  client_t *client = (client_t *)cli;
  char name[NAME_SIZE];
  volatile sig_atomic_t flag=0;
  char buffer[BUFFER_SIZE];
  clientCount++;

  if(recv(client->sockfd, name, NAME_SIZE, 0)<=0 || strlen(name)<2 || strlen(name)>=NAME_SIZE-1)
    printf("[-] Didn't enter the name \n");


  fflush(stdin);
  fflush(stdout);

  
  strcpy(client->name, name);
  printf("%s has slide in to chat. \n", client->name);
  sprintf(buffer, "\n %s has slide in to chat. \n", client->name);
  sendall(client->id, buffer);

  while(flag){

    int n=recv(client->sockfd,buffer,BUFFER_SIZE, 0);
    if(n<0)
      error("[-] Error in receiving \n");
    buffer[n]='\0';

    if(strncmp(buffer,"bye",3)==0){
      flag=1;
      sprintf(buffer, "%s has left the chat. \n", client->name);
      printf("%s has left the chat. \n", client->name);
      sendall(client->id, buffer);
      break;
    }
    else if(strncmp(buffer,"kill",4)==0){
      sprintf(buffer,"[-]Server has been terminated by %s \n", client->name);
      terminator=1;
      flag=1;
      sendall(client->id, buffer);
      printf("[-]Server has been terminated by %s \n", client->name);
      break;
    }

    else{
        char * ptr= strtok(buffer," ");
        int a=atoi(ptr);
        int op;
        int b;
        while(ptr!=NULL){
          ptr=strtok(NULL," ");
          if(ptr[0]=='+' )
              op=0;
          else if(ptr[0]=='-')
              op=1;
          else if(ptr[0]=='*')
              op=2;
          else if(ptr[0]=='/')
              op=3;
          else
              b=atoi(ptr);
        }
        char ans[10];

        switch(op){
          case 0:
            sprintf(ans,"%d",a+b);
            break;
          case 1:
            sprintf(ans,"%d",a-b);
            break;
          case 2:
            sprintf(ans,"%d",a*b);
            break;
          case 3:
            sprintf(ans,"%d",a/b);
            break;
        }
        char msg[BUFFER_SIZE+NAME_SIZE+30];
        sprintf(msg, "%s: %s = %s\n", client->name, buffer, ans);
        printf("%s", msg);
      } 
  }

  close(client->sockfd);
  popclient(client->id);
  clientCount--;
  pthread_detach(pthread_self());   
  return NULL;
}


int main(){

  sockfd=socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in server_addr, client_addr;

  server_addr.sin_family=AF_INET;
  server_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
  server_addr.sin_port=htons(PORT);


  signal(SIGPIPE, SIG_IGN);

  if(setsockopt(sockfd, SOL_SOCKET,( SO_REUSEADDR|SO_REUSEADDR ), &(int){1}, sizeof(int))<0)
    error("[-]ERROR setting socket options");


  if(bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr))<0)
    error("[-] Error in binding \n");
  
  if(listen(sockfd, MAX_CLIENTS)<0)
    error("[-] Error in listening \n");

  printf("[+] Server started \n");

  socklen_t cli_len=sizeof(client_addr);

  while(!terminator){

    int clientfd=accept(sockfd, (struct sockaddr *)&client_addr, &cli_len);
    if(clientfd<0)
      error("[-] Error in accepting \n");

    if(clientCount+1==MAX_CLIENTS){
      printf("[-] Max clients reached. Rejected: \n");
      printf("%s:%d \n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
      close(clientfd);
      continue;
    }

    client_t *client=(client_t *)malloc(sizeof(client_t));
    client->address=client_addr;
    client->sockfd=clientfd;
    client->id=uid++;

    pushclient(client);
    pthread_t th;

    if(pthread_create(&th, NULL, handler, (void *)client)<0)
      error("[-] Error in creating thread \n");

  }

  close(sockfd);


  return 0;
}