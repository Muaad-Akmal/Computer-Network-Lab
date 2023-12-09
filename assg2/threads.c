#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

int arr[10]={1,2,3,4,5,6,7,8,9,10};

void *routine(void * arg)
{
  sleep(1);
  int index=*(int *)arg;
  int *x=malloc(sizeof(int));
  *x=arr[index];
  return (void **)x;
}

int main(){

  pthread_t th[10];

  for (int i = 0; i < 10; i++)
  {
    int *arg=malloc(sizeof(*arg));
    *arg=i;
    pthread_create(th+i, NULL, &routine, (void *)arg );
  }

  for (int i = 0; i < 10; i++)
  {
    void  *x;
    pthread_join(th[i], x);
    printf("%d ",*(int *)x);
  }
  


}