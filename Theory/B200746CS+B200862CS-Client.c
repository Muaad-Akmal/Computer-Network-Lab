#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

#define PORT 6666
#define MESSAGE_SIZE 1024
#define PACKET_SIZE 3000
#define IP "127.0.0.1"

typedef struct packet{
  int type;   // 1 or 2
  int seq;    
  char payload[1024];
  long long checksum;
}packet;


int sockfd;
struct sockaddr_in serverAddr;
volatile sig_atomic_t flag=0;


long long int checksum(packet p, int len)
{
    unsigned long long int checksum = 0;
    for (int i = 0; i < len; i++)
    {
        checksum ^= p.payload[i];
    }

    checksum ^= p.type;
    checksum ^= p.seq;

    return checksum;
}

static int p1_count ;
static int p2_count ;


void error(char *msg){
  perror(msg);
  exit(1);
}


void * packet1_handler(){

  socklen_t servlen = sizeof(serverAddr);
  int n;
  while(1){
    packet p1;
    p1.type = 1;
    p1.seq = (p1_count++);

    strcpy(p1.payload, "packet type 1");

    p1.checksum = checksum(p1, sizeof(p1.payload));
    
    printf("PACKET- 1\n");
    fflush(stdout);

    // destructuring packet to a string
    char p1_str[PACKET_SIZE];
    sprintf(p1_str, "%d*%d*%s*%lld*", p1.type, p1.seq, p1.payload, p1.checksum);

    n = sendto(sockfd, p1_str, strlen(p1_str), 0 , (struct sockaddr *) &serverAddr, servlen);
    if(n<0)
      error("[-] Error in sending \n");

    usleep(100000);
  }
  return NULL;
}

void * packet2_handler(){

  int n;
  socklen_t servlen = sizeof(serverAddr);
  while(1){
    packet p2;
    p2.type = 2;
    p2.seq = (p2_count++);

    strcpy(p2.payload, "packet type 2");

    p2.checksum = checksum(p2, sizeof(p2.payload));
    
    printf("              PACKET- 2\n");
    fflush(stdout);


    // destructuring packet to a string
    char p2_str[PACKET_SIZE];
    sprintf(p2_str, "%d*%d*%s*%lld*", p2.type, p2.seq, p2.payload, p2.checksum);

    n = sendto(sockfd, p2_str, PACKET_SIZE, 0 , (struct sockaddr *) &serverAddr, servlen);
    if(n<0)
      error("[-] Error in sending \n");

    usleep(150000);
  }
  return NULL;
}




int main(){

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if(sockfd<0)
    error("[-] Error in socket \n");



  printf("[+]-------Client started-------\n");

  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(PORT);
  serverAddr.sin_addr.s_addr = inet_addr(IP);

  if (connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
    error("[-] Error in connection \n");
 
  printf("[+]Connected to Server \n");

  pthread_t packet1 , packet2;

  pthread_create(&packet1, NULL, packet1_handler, NULL);
  pthread_create(&packet2, NULL, packet2_handler, NULL);

  while(1){
    if(flag){
      printf("[-]Disconnected from server \n");
      break;
    }
  }

  close(sockfd);


  return 0;
}


