#include <stdio.h>
#include <math.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#define NUM_THREADS 5;

typedef struct data
{
	int size;
	int* distances;
	int* selected_vertices;
	int* vertices;
	int** graph;
	int closest_vertex;
	int no_of_threads;
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
	printf("This program computes the minimum spanning tree of a weighted dense graph using pthreads.\n");
	if(argc != 3){
		printf("Usage: ./threads <no_of_nodes> <no_of_threads>\n");
		exit(0);
	}
	int size = atoi(argv[1]);
	int no_of_threads = atoi(argv[2]);
	

	int **mst;
	int i,j;
	Data data;
	ThreadData threadData[no_of_threads];
	pthread_t threads[no_of_threads];	
	struct timeval start_time, end_time;		
	
	data.size = size;
	data.no_of_threads = no_of_threads;
	data.distances = malloc(size * sizeof(int));
	data.vertices = malloc(size * sizeof(int));
	data.selected_vertices = malloc(size * sizeof(int));
	data.graph = (int**) malloc(size*sizeof(int*));
	mst = (int**) malloc(size*sizeof(int*));
	
	for(i = 0; i < size; i++){
		data.graph[i] = (int*) malloc(size*sizeof(int));
		mst[i] = (int*) malloc(size*sizeof(int));
	}

	for(i = 0; i < no_of_threads; i++){
		threadData[i].data_pointer = (struct Data *)&data;
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
	gettimeofday(&start_time, NULL);
	// pick a random vertex to begin with
	int no_of_selected_vertices = 1;
	int first_vertex = rand() % size;
	data.selected_vertices[0] = first_vertex;
	for(i = 0; i < size; i++){
		data.distances[i] = data.graph[first_vertex][i];
		data.vertices[i] = first_vertex;
	}
	
	while(no_of_selected_vertices <= size){
		int v = closest_vertex(data.distances,data.selected_vertices,size);
		if(v == -1)
			break;
		data.closest_vertex = v;
		//printf("closest_vertex: %d\n ",data.closest_vertex);
		data.selected_vertices[no_of_selected_vertices] = data.closest_vertex;
		no_of_selected_vertices++;
		mst[data.closest_vertex][data.vertices[data.closest_vertex]] = 1;		
		//printf("no of vertices selected so far:%d\n",no_of_selected_vertices);
		int rc;
		int t;
		for(t=0; t<no_of_threads; t++){
			rc = pthread_create(&threads[t], NULL, updateDistances, (void*) &threadData[t]);
			if (rc){
				 printf("ERROR; return code from pthread_create() is %d\n", rc);
			 	exit(-1);
			}
			//printf("before join\n");
			pthread_join(threads[t],NULL);			
			//printf("after join\n");
		}
		//printf("after for loop\n");
	}
	gettimeofday(&end_time, NULL);
	int mst_length = 0;
	for(i = 0; i < size; i++){
		for(j = 0; j < size; j++){			
			if(mst[i][j] == 1){
				//printf("length of edge %d-%d is %d\n", i,j,data.graph[i][j]);
				mst_length += data.graph[i][j];
			}
		}		
	}
	printf("MST length: %d\n",mst_length);
	printf("time taken: %ld\n", ((end_time.tv_sec * 1000000 + end_time.tv_usec) - (start_time.tv_sec * 1000000 + start_time.tv_usec)));
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
	int i;
	int vertex = -1;
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
	ThreadData* threadData = (struct ThreadData *)d;
	Data* data = threadData->data_pointer;	
	int i;
	int start,end;
	int offset = data->size/ data->no_of_threads;
	start = threadData->thread_id * offset;
	end = (threadData->thread_id + 1) * offset - 1;
	// printf("thread_id: %d; start:%d; end: %d; closest_vertex: %d\n",threadData->thread_id,start,end, closest_vertex);
	for(i = start; i <= end; i++){
		if(in_array(data->selected_vertices,data->size,i) == 0){
			int v = data->closest_vertex;
			if(data->graph[i][v] != 0 && data->distances[i] > data->graph[i][v]){
				data->distances[i] = data->graph[i][v];	
				data->vertices[i] = v;
			}
		}
	}
	//printf("exiting thread %d\n", threadData->thread_id);
	pthread_exit(NULL);
}