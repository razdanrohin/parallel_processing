#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <omp.h>
#include "sort.h"
#include "edgelist.h"
#define NUMBER_OF_THREADS 4

// Order edges by id of a source vertex,
// using the Counting Sort
// Complexity: O(E + V)

//========================================================================================================================

int power(int num){
    int tenth=1;
    if (num==0){
        return tenth;
    }
    else{
        while (num>0){
            tenth=tenth*10;
            num--;
        }
    }
    return tenth;
}
//========================================================================================================================
int numDigits(int number)
{
    int digits = 0;
    if (number < 0) digits = 1; // remove this line if '-' counts as a digit
    while (number) {
        number /= 10;
        digits++;
    }
    return digits;
}
//=========================================================================================================================

int nthDigit(int number, int index){
    int digit=0;
    if (index<numDigits(number)){
        digit = (number/ power(index)) % 10;
    }
    return digit;
}
//=========================================================================================================================

void countSortEdgesBySource(struct Edge *edges_sorted, struct Edge *edges, int numVertices, int numEdges) {
    int i;
    int key;
    int pos;

    // auxiliary arrays, allocated at the start up of the program
    int *vertex_cnt = (int*)malloc(numVertices*sizeof(int)); // needed for Counting Sort
 
    for(i = 0; i < numVertices; ++i) {
        vertex_cnt[i] = 0;
    }
    // count occurrence of key: id of a source vertex
    for(i = 0; i < numEdges; ++i) {
        key = edges[i].src;
        vertex_cnt[key]++;
    }
    // transform to cumulative sum
    for(i = 1; i < numVertices; ++i) {
        vertex_cnt[i] += vertex_cnt[i - 1];
    }

    // fill-in the sorted array of edges
    for(i = numEdges - 1; i >= 0; --i) {
        key = edges[i].src;
        pos = vertex_cnt[key] - 1;
        edges_sorted[pos] = edges[i];
        vertex_cnt[key]--;
    }
    free(vertex_cnt);
}


void radixSortEdgesBySource(struct Edge *edges_sorted, struct Edge *edges, int numVertices, int numEdges) {
    int num_threads = omp_get_thread_num();
    int length_Num=numDigits(numVertices);
    int number_Vertices=10; //0-9
    printf("\n%d\n",num_threads);
    omp_set_num_threads(2);
    countSortEdgesBySourceRadix(edges_sorted, edges, number_Vertices, numEdges, length_Num, 0);
}

// The following function is modified from the above countSortEdgesBySource function to the radix implementation with two extra 
//arguments
void countSortEdgesBySourceRadix(struct Edge *edges_sorted, struct Edge *edges, int numVertices, int numEdges, int totalIndex, int index) {

    int stop=index+1;
    int i;

    // auxiliary arrays, allocated at the start up of the program
    int *vertex_cnt = (int*)malloc(numVertices*sizeof(int)); // needed for Counting Sort


//mention in the document why we have to use the static parallelism over here rather than the dynamic
//why is it saving the memory
    //omp_set_num_threads(NUMBER_OF_THREADS);
    //
  //  #pragma omp parallel for schedule(dynamic,4)
    for(i = 0; i < numVertices; ++i) {
        vertex_cnt[i] = 0;
    }


    for(i = 0; i < numEdges; ++i) {
      vertex_cnt[nthDigit(edges[i].src,index)]++;
    }
//increase or decrease the value of this variable to see how fast the conversion is taking place
//play around with this pragma function

//here after increasing the number of threads
#pragma omp parallel for schedule(dynamic,2)
for(i = 1; i < numVertices; ++i)
{
        vertex_cnt[i] += vertex_cnt[i - 1];

}

#pragma omp barrier

//#pragma omp critical
{
    if (index%2==0){
        for(i = numEdges - 1; i >= 0; --i) {
                edges_sorted[vertex_cnt[nthDigit(edges[i].src,index)] - 1] = edges[i];
                vertex_cnt[nthDigit(edges[i].src,index)]--;
        }
    }
    else{
        for(i = numEdges - 1; i >= 0; --i) {

            edges[vertex_cnt[nthDigit(edges_sorted[i].src,index)]-1]=edges_sorted[i];
            vertex_cnt[nthDigit(edges_sorted[i].src,index)]--;
        }

    }

}
#pragma omp barrier

free(vertex_cnt);

    if (stop<totalIndex){
        countSortEdgesBySourceRadix(edges_sorted, edges,  numVertices, numEdges, totalIndex, stop);
    }
}


