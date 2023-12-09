#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <signal.h>
#include <arpa/inet.h>



#define PORT 6666
#define MESSAGE_SIZE 1024
#define PACKET_SIZE 3000
#define IP "127.0.0.1"

static _Atomic unsigned int p1_count;
static _Atomic unsigned int p2_count;

void error(char *msg){
  perror(msg);
  exit(1);
}

int sockfd;
struct sockaddr_in serverAddr;

pthread_mutex_t Mutex1 = PTHREAD_MUTEX_INITIALIZER;


void * packet1_handler(void *packet){
  
  char *packet_ = (char *)packet;
  char p[PACKET_SIZE];

  strcpy(p , packet_);
  p[strlen(p)-1] = '\0';

  // printf("Packet: %s\n", p);
  // printf("%s\n", p);

  // pthread_mutex_lock(&Mutex1);

  int checksum=0;
  int n;
  char *payload;
  //destructuring packet string
  int type , seq;
  char *temp=NULL;

  fflush(stdout);


  temp = strtok(p,"*");
  
  type = atoi(temp);
  
    if(type==1)
      p1_count++;
    else  
      return NULL;

  temp = strtok(NULL,"*");
  seq = atoi(temp);
  
  temp = strtok(NULL,"*");
  payload = temp;
  
  temp = strtok(NULL,"*");
  checksum = atoi(temp);

  printf("\n");
  printf("[+]Packet 1 received\n");
  printf("Type: %d\n",type);
  // printf("Seq: %d\n",seq);
  // printf("Payload: %s\n",payload);
  // printf("Checksum: %d\n",checksum);
  printf("\n");

  fflush(stdout);

// pthread_mutex_unlock(&Mutex1);

  return NULL;
}

pthread_mutex_t Mutex2 = PTHREAD_MUTEX_INITIALIZER;

void * packet2_handler(void *packet){
  
  int checksum=0;
  
  char *packet_ = (char *)packet;
  char p[PACKET_SIZE];
  strcpy(p , packet_);

  // printf("%s\n", p);
  // pthread_mutex_lock(&Mutex2);

  int n;
  char *payload;
  //destructuring packet string 
  int type , seq;
  char *temp=NULL;

  // p = "2*1*hello*123*"

  temp = strtok(p,"*");
  type = atoi(temp);

  //type = 2
  
  if(type==2)
    p2_count++;
  else  
    return NULL;



  temp = strtok(NULL,"*");
  seq = atoi(temp);

  //seq = 1

  temp = strtok(NULL,"*");
  payload = temp;
  
  //payload = "hello"

  temp = strtok(NULL,"*");
  checksum = atoi(temp);

  //checksum = 123

  printf("[+]Packet 2 received\n");
  printf("Type: %d\n",type);
  // printf("Seq: %d\n",seq);
  // printf("Payload: %s\n",payload);
  // printf("Checksum: %d\n",checksum);
  printf("\n");

  // pthread_mutex_unlock(&Mutex2);

  return NULL;
}

pthread_mutex_t Mutex3 = PTHREAD_MUTEX_INITIALIZER;

void * error_handler(void *packet){

  
  char *packet_ = (char *)packet;
  char p[PACKET_SIZE];
  strcpy(p , packet_);

  // printf("%s\n", p);
  pthread_mutex_lock(&Mutex3);

  int n;
  char *payload;
  //destructuring packet string 
  int type , seq;
  char *temp=NULL;

  // pthread_mutex_lock(&Mutex3);

  temp = strtok(p,"*");
  type = atoi(temp);
  
    temp = strtok(p,"*");
  type = atoi(temp);
  
  if(type==2)
    p2_count++;
  else  
    return NULL;

  temp = strtok(NULL,"*");
  seq = atoi(temp);
  
  temp = strtok(NULL,"*");
  payload = temp;
  
  temp = strtok(NULL,"*");
  int checksum = atoi(temp);


  int checksum_calc=0;

  for(int i=0;i<strlen(payload);i++){
    checksum_calc ^= payload[i];
  }

  checksum_calc ^= type;
  checksum_calc ^= seq;


  if(checksum_calc==checksum)
      return  (void *) 1;
  else{
    printf("[-]Packet Corrupted\n");
    printf("Type: %d\n",type);
    printf("Seq: %d\n",seq);
    printf("Payload: %s\n",payload);
    printf("Checksum: %d\n",checksum);
    printf("\n");
    return  (void *) 0;
  }

  pthread_mutex_unlock(&Mutex3);


}


void * status_handler(void *packet){

  while(1){
    printf("=============Packet Status=============\n");
    printf("Packet1 count: %d\n",p1_count);
    printf("Packet2 count: %d\n",p2_count);
    printf("\n");
    printf("=======================================\n");
    printf("\n");
     usleep(300000);
    // sleep(4);
  }
  return NULL; 
}

//q: how to sleep for 150ms in c using usleep
//a:  usleep(300000);


int main(){

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if(sockfd<0){
    error("Error opening socket");
  }

  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(PORT);
  serverAddr.sin_addr.s_addr = inet_addr(IP);

  socklen_t servlen = sizeof(serverAddr);

  signal(SIGPIPE, SIG_IGN);

  if(setsockopt(sockfd, SOL_SOCKET,( SO_REUSEADDR|SO_REUSEADDR ), &(int){1}, sizeof(int))<0)
    error("[-]ERROR setting socket options");

  printf("[+]---- Server Live ----[+]\n");

  if(bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr))<0){
    error("[-]ERROR binding socket");
  }


  char packet[PACKET_SIZE];
  int n;
  pthread_t errorth , p1th, p2th, statusth;
  pthread_create(&statusth, NULL, status_handler, NULL);
  int flag=0;


    while(1){
      bzero(packet, PACKET_SIZE);
      n = recvfrom(sockfd, packet, PACKET_SIZE, 0, (struct sockaddr*)&serverAddr, &servlen);
    // printf("%s\n", packet);

      fflush(stdout);

      pthread_create(&errorth, NULL, error_handler, packet);
      
      // int corrupted;
      // pthread_join(errorth, (void *)&corrupted);
      
      // if(corrupted==0)
      //   continue;

      pthread_create(&p1th, NULL, packet1_handler, packet);
      pthread_create(&p2th, NULL, packet2_handler, packet);
    
    fflush(stdout);
 
      pthread_join(p1th, NULL);
      pthread_join(p2th, NULL);
      // sleep(1);
  }



  close(sockfd);
                                      
  return 0;
}



