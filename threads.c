#include <stdio.h>
#include <math.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <pthread.h>
#define NUM_THREADS 5;

typedef struct data
{
	int size;
	int* distances;
	int* selected_vertices;
	int* vertices;
	int** graph;
	int closest_vertex;
} Data;

typedef struct threadData{
	struct Data* data_pointer;
	int thread_id;
} ThreadData;

int closest_vertex(int distances[], int selected_vertices[], int size);
int in_array(int selected_vertices[], int size, int ele);
int min(int a, int b);
void *updateDistances(void* d);

int main(int argc, char *argv[]){
	printf("This program computes the minimum spanning tree of a weighted dense graph using serial computation.\n");
	printf("Enter the number of nodes in the graph\n");
	int size;
	int **mst;
	int i,j;
	Data data;
	ThreadData threadData[5];
	scanf("%d",&size);
	data.size = size;
	data.distances = malloc(size * sizeof(int));
	data.vertices = malloc(size * sizeof(int));
	data.selected_vertices = malloc(size * sizeof(int));
	data.graph = (int**) malloc(size*sizeof(int*));
	mst = (int**) malloc(size*sizeof(int*));
	
	for(i = 0; i < size; i++){
		data.graph[i] = (int*) malloc(size*sizeof(int));
		mst[i] = (int*) malloc(size*sizeof(int));
	}

	for(i = 0; i < 5; i++){
		threadData[i].data_pointer = &data;
		threadData[i].thread_id = i;
	}
		
	

	
	for(i = 0; i < size; i++)
		for(j = 0; j < size; j++)
			mst[i][j] = 0;

	// generating random graph
	/*
	for(i=0; i<size; i++){ 
		for(j=0; j<size; j++){
			if(i == j){
				graph[i][i] = 0;
				continue;
			}
			int r = rand() % 100;			
			graph[i][j] = r;
			graph[j][i] = r;
		}		
	}
	*/
	i = 0;
	j = 0;
	FILE *file = fopen("graph.txt", "r");
	while(fscanf(file,"%d",&j) == 1 && i < size * size){
		data.graph[i/size][i%size] = j;
		i++;
	}
	fclose(file);	

    // print the graph
    /*	
	for(i=0; i<size; i++){ 
		for(j=0; j<size; j++){
			printf("%d\t",graph[i][j]);
		}		
		printf("\n");
	}
	*/

	// make sure the selected vertices array is empty
	for(i = 0; i < size; i++)
		data.selected_vertices[i] = -1;
	// pick a random vertex to begin with
	int no_of_selected_vertices = 1;
	int first_vertex = rand() % size;
	data.selected_vertices[0] = first_vertex;
	for(i = 0; i < size; i++){
		data.distances[i] = data.graph[first_vertex][i];
		data.vertices[i] = first_vertex;
	}
	
	while(no_of_selected_vertices != size){
		data.closest_vertex = closest_vertex(data.distances,data.selected_vertices,size);
		data.selected_vertices[no_of_selected_vertices] = data.closest_vertex;
		no_of_selected_vertices++;
		mst[data.closest_vertex][data.vertices[data.closest_vertex]] = 1;
		
		pthread_t threads[5];
		int rc;
		long t;
		for(t=0; t<5; t++){
 			rc = pthread_create(&threads[t], NULL, updateDistances, (void*) &threadData[t]);
			if (rc){
				 printf("ERROR; return code from pthread_create() is %d\n", rc);
			 	exit(-1);
			}
		}		
	}

	int mst_length = 0;
	for(i = 0; i < size; i++){
		for(j = 0; j < size; j++){			
			if(mst[i][j] == 1){
				printf("length of edge %d-%d is %d\n", i,j,data.graph[i][j]);
				mst_length += data.graph[i][j];
			}
		}		
	}
	printf("MST length: %d",mst_length);
}	

int in_array(int selected_vertices[], int size, int ele){
	int i = 0;
	for(i = 0; i < size ; i++){
		if(selected_vertices[i] == ele){
			// printf("%d is ALREADY there\n", ele);
			return 1;
		}			
	}
	// printf("%d is NOT there\n", ele);
	return 0;
}	

int min(int a, int b) { return (a < b) ? a : b; }

int closest_vertex(int distances[], int selected_vertices[], int size){
	int vertex,i;
	int min_distance = INT_MAX;
	for(i = 0; i < size; i++){
		if(in_array(selected_vertices,size,i) == 0){
			int len = distances[i];
			if(len != 0 && len < min_distance){
				vertex = i;
				min_distance = distances[i];
			}				
		}
	}
	return vertex;
}

void *updateDistances(void* d){  
	ThreadData* threadData = (struct ThreadData*)d;
	Data* data = threadData->data_pointer;	
	int i;
	int start,end;
	int offset = data->size/ 5;
	start = threadData->thread_id * offset;
	end = (threadData->thread_id + 1) * offset - 1;
	printf("thread_id: %d; start:%d; end: %d; closest_vertex: %d\n",threadData->thread_id,start,end, closest_vertex);
	for(i = start; i <= end; i++){
		if(in_array(data->selected_vertices,data->size,i) == 0){
			int v = data->closest_vertex;
			if(data->graph[i][v] != 0 && data->distances[i] > data->graph[i][v]){
				data->distances[i] = data->graph[i][v];	
				data->vertices[i] = v;
			}
		}
	}
	pthread_exit(NULL);
}