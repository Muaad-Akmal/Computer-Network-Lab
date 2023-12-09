// c code for bellman ford algo using adjacency matrix

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define INF 1000000


int main(){
  //bellman ford

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
  



}
