#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <vector>

// #define _DEBUG
#define MAX_NODE_SIZE 1000

#define COST_INF 255

#ifdef _DEBUG
#define LOGF  printf
#else
#define LOGF(...)
#endif

typedef unsigned char uint_8;
typedef unsigned short uint_16;
typedef unsigned long uint_32 ;

typedef struct Link_Info_t {
    uint_8 bandwidth;
    uint_8 cost;
}Link_Info;

typedef std::vector<Link_Info> Adjacency_Matrix_Row;
typedef std::vector<Adjacency_Matrix_Row> Adjacency_Matrix;

#endif
