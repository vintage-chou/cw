#ifndef __LIB_UTIL_H__
#define __LIB_UTIL_H__

#include <stdio.h>
#include <vector>
#include "config.h"

/* typedef struct MGraph_t {
    int max_size;
    Adjacency_Matrix matrix;
}MGraph; */

// typedef std::vector<int> Path_Matrix;
// typedef std::vector<int> Shortest_Path;

typedef uint_16 Path_Matrix;
typedef uint_16 Shortest_Path;

void shortest_dijkstra(Adjacency_Matrix m, int size, int v0, Path_Matrix p[], Shortest_Path s[]);

#endif
