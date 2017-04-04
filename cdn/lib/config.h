#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <vector>
using namespace std;

// #define _DEBUG
// #define _MY_DEBUG

#define DETIL_FEEDBACK 0

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
typedef unsigned long uint_32;
typedef unsigned long long uint_64 ;

typedef struct Element_t
{
    uint_16 bandwidth;
    short flow;
    uint_16 left;        // left bandwidth
    char cost;
    char origin_cost;
} Element;

typedef struct _Affinity_Info
{
  uint_16 line_cost;
  uint_16 flow_err;
}Affinity_Info;

typedef vector<vector <uint_32> > Feedback;

typedef vector<Element> Adjacency_Matrix_Row;
typedef vector<Adjacency_Matrix_Row> Adjacency_Matrix;

typedef vector<uint_16> Src_Tar_Set;
typedef vector<uint_16> Output_File_Info_Row;
typedef vector<vector<uint_16> > Output_File_Info;
typedef vector<vector<uint_16> > Input_File_Info;

typedef vector<vector<uint_16> > FLOW;
typedef vector<vector<uint_16> > CUSTOMER;

#endif
