#include <stdio.h>
#include <stdlib.h>


int main(){



  int n;
  char *payload;
  //destructuring packet string 
  int type , seq;
  char *temp=NULL;

  char p[100] = "2*1*hello pal*123*";

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
  printf("Seq: %d\n",seq);
  printf("Payload: %s\n",payload);
  printf("Checksum: %d\n",checksum);
  printf("\n");


}