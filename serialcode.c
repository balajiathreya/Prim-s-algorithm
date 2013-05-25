#include <stdio.h>
#include <math.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int closest_vertex(int distances[], int selected_vertices[], int size);
int in_array(int selected_vertices[], int size, int ele);
int min(int a, int b);

int main(int argc, char *argv[]){
	printf("This program computes the minimum spanning tree of a weighted dense graph using serial computation.\n");
	if(argc != 2){
		printf("Usage: ./serialcode <no_of_nodes>\n");
		exit(0);
	}
	int size = atoi(argv[1]);
	
	int selected_vertices[size], distances[size], vertices[size];
	
	int graph[size][size],mst[size][size];
	int i,j;
	
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
	int record = 0;
	int tmp;
	FILE *file = fopen("graph.txt", "r");
	while(fscanf(file,"%d",&tmp) == 1 && record < size * size){
		graph[record/size][record%size] = tmp;
		record++;
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
		selected_vertices[i] = -1;
	// pick a random vertex to begin with
	int no_of_selected_vertices = 1;
	int first_vertex = rand() % size;
	selected_vertices[0] = first_vertex;
	for(i = 0; i < size; i++){
		distances[i] = graph[first_vertex][i];
		vertices[i] = first_vertex;
	}
	
	int v;
	while(no_of_selected_vertices != size){
		v = closest_vertex(distances,selected_vertices,size);
		selected_vertices[no_of_selected_vertices] = v;
		no_of_selected_vertices++;
		mst[v][vertices[v]] = 1;

		for(i = 0; i < size; i++){
			if(in_array(selected_vertices,size,i) == 0){
				if(graph[i][v] != 0 && distances[i] > graph[i][v]){
					distances[i] = graph[i][v];	
					vertices[i] = v;
				}
			}
		}		
	}

	int mst_length = 0;
	for(i = 0; i < size; i++){
		for(j = 0; j < size; j++){			
			if(mst[i][j] == 1){
				printf("length of edge %d-%d is %d\n", i,j,graph[i][j]);
				mst_length += graph[i][j];
			}
		}		
	}
	printf("MST length: %d\n",mst_length);
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