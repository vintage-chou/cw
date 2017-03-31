#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <vector>

// #define _DEBUG
#define _MY_DEBUG

#define MAX_NODE_SIZE 1000

#define COST_INF 120
#define BANDWIDTH_INF (uint_16(-1))
#define NO_PARENT 1100

#ifdef _MY_DEBUG
#define LOGF  printf
#else
#define LOGF(...)
#endif

typedef unsigned char uint_8;
typedef unsigned short uint_16;
typedef unsigned long uint_32 ;

typedef struct Link_Info_t {
    uint_16 bandwidth;
    char cost;
}Link_Info;

typedef std::vector<Link_Info> Adjacency_Matrix_Row;
typedef std::vector<Adjacency_Matrix_Row> Adjacency_Matrix;

typedef std::vector<uint_16> Src_Tar_Set;
typedef std::vector<std::vector<uint_16> > Output_File_Info;
typedef std::vector<std::vector<uint_16> > Input_File_Info;


#endif
