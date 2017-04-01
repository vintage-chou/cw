#ifndef __LIB_UTIL_H__
#define __LIB_UTIL_H__

#include <stdio.h>
#include <vector>
#include "config.h"

typedef std::vector<uint_16> Path_Matrix;
// TODO: optimalize the type of Shortest_Path
typedef uint_16 Shortest_Path;

typedef bool (*Shortest_Func) (Adjacency_Matrix const &, uint_16, uint_16, Path_Matrix &, Shortest_Path []);

bool shortest_dijkstra(Adjacency_Matrix const &m, uint_16 src, uint_16 tar, Path_Matrix &path, Shortest_Path shortest_length[]);
bool shortest_spfa(Adjacency_Matrix const &m, uint_16 src, uint_16 tar, Path_Matrix &path, Shortest_Path shortest_length[]);
void super_ford_fulkerson(Adjacency_Matrix &am, Output_File_Info &of);
void ford_fulkerson(Adjacency_Matrix &am, Shortest_Func fsp, uint_16 src, uint_16 tar);
void print_matrix(Adjacency_Matrix const &am);

#endif
