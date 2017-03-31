#ifndef __LIB_UTIL_H__
#define __LIB_UTIL_H__

#include <stdio.h>
#include <vector>
#include "config.h"

/* typedef struct MGraph_t {
    int max_size;
    Adjacency_Matrix matrix;
}MGraph; */

typedef struct Element_t
{
    uint_16 bandwidth;
    uint_16 flow;
    uint_16 left;        // left bandwidth
    char cost;
} Element;

typedef std::vector<Element> Extern_Adjacency_Matrix_Row;
typedef std::vector<Extern_Adjacency_Matrix_Row> Extern_Adjacency_Matrix;

typedef std::vector<uint_16> Path_Matrix;
// TODO: optimalize the type of Shortest_Path
typedef uint_16 Shortest_Path;

typedef bool (*Shortest_Func) (Extern_Adjacency_Matrix const &, uint_16, uint_16, Path_Matrix &, Shortest_Path []);

bool shortest_dijkstra(Extern_Adjacency_Matrix const &m, uint_16 src, uint_16 tar, Path_Matrix &path, Shortest_Path shortest_length[]);
bool shortest_spfa(Extern_Adjacency_Matrix const &m, uint_16 src, uint_16 tar, Path_Matrix &path, Shortest_Path shortest_length[]);
void am2eam(Adjacency_Matrix const &am, Extern_Adjacency_Matrix &eam);
void super_ford_fulkerson(Adjacency_Matrix const &am, Src_Tar_Set const &s, Src_Tar_Set const &t);
void ford_fulkerson(Extern_Adjacency_Matrix &eam, Shortest_Func fsp, uint_16 src, uint_16 tar);

#endif
