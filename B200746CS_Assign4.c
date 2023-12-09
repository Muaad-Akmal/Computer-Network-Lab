#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

#define INF 10000

//Distance vector routing Algorithm = Bellman ford routing algo
// ARPANET = Advanced Research Projects Agency Network
//RIP : Routing Information Protocol

//ht be number of hops, time delay in milliseconds, total number of packets 
// queued along the path,

// q : what is count to infinity problem ?
// bad news spread slowly, good news spread fast



int main(){

  int n;
  printf("Enter the number of nodes in the network: ");
  scanf("%d", &n);

  int mat[n][n];

  for(int i=0; i<n; i++){
    for(int j=0; j<n; j++){
      if(i==j)
        mat[i][j] = 0;
      else
        mat[i][j] = INF;
    }
  }

  int E;
  printf("Enter the number of edges in the network: ");
  scanf("%d", &E);

  int parent[n];
  memset(parent, -1, sizeof(parent));


  int u, v, w;
  for(int i=0; i<E; i++){
    scanf("%d %d %d", &u, &v, &w);
    mat[u][v] = w;
    mat[v][u] = w;
  }


  int pathtracer[n][n];
  for(int i=0; i<n; i++){
    for(int j=0; j<n; j++){
      pathtracer[i][j] = j;
    }
  }

//---------------------Bellman Ford---------------------

  for (int round = 0; round < n-1; round++)
  {
    int flag=0;
    for (int i = 0; i < n; i++)
    {
      for (int j = 0; j < n; j++)
      {     
        for (int k = 0; k < n; k++)
        {
          
          if(mat[i][j] > mat[i][k] + mat[k][j]){
            flag=1;
            mat[i][j] = mat[i][k] + mat[k][j];
            pathtracer[i][j] = pathtracer[i][k];
          }
        }
      }
    }
    if(flag==0)
      break;
  }

  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < n; j++)
    {
      if(mat[i][j]!=INF)
        printf("%d  ", mat[i][j]);
      else
        printf("-1  ");
    }
    printf("\n");
  }
    printf("\n");

//---------------------Routing Table---------------------

    for (int i=0; i<n; i++) {
        printf("\nRouting Table @ Router %d\n", i);
        printf("+------+------+------+\n");
        printf("| Dest | Next | Cost |\n");
        printf("+------+------+------+\n");
        for (int j=0; j<n; j++) {
                printf("|%5d |%5d |%5d |\n", j, pathtracer[i][j], mat[i][j]);
        }
        printf("+------+------+------+\n");
    }


  return 0;
}
