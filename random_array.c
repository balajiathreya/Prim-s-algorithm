#include <stdio.h>
#include <math.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
int main(int argc, char *argv[]){
	int size = atoi(argv[1]);	
	int graph[size][size];
	int i,j;
	
	// generating random graph
	for(i=0; i<size; i++){ 
		for(j=0; j<size; j++){
			if(i == j){
				graph[i][j] = 0;
				continue;
			}
			int r = rand() % 100;			
			graph[i][j] = r;
			graph[j][i] = r;
		}		
	}

	for(i=0; i<size; i++){ 
		for(j=0; j<size; j++){
			printf("%d\t",graph[i][j]);
		}		
		printf("\n");
	}
}