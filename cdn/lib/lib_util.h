#include <stdio.h>
#include <vector>
#include "config.h"

typedef std::vector<std::vector<int> > MGraph;
// typedef int Matrix[MAX_NODE_SIZE][MAX_NODE_SIZE];

typedef struct MGraph_t {
    int max_size;
    Matrix matrix;
}MGraph;

int shortest_dijkstra(MGraph m, int v0, int path_matrix[], int sp_table[]);
