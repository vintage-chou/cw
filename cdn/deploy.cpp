#include "deploy.h"
#include "lib_util.h"
#include "config.h"

#include <stdio.h>
#include <vector>
#include <iostream>
#include <string.h>

#define MAX_LINE_LEN 55000

#define START 0
#define END   1
#define BANDWIDTH 2
#define COST 3

using namespace std;

typedef struct Global_Info_t {
    int line_num;
    int network_node;
    int links;
    int customer_node;
    int server_cost;
}Global_Info;

typedef vector<vector<uint_16> > Input_File_Info;

#define MAX_NODE_COSUMER_SIZE 1500

static Path_Matrix path[MAX_NODE_COSUMER_SIZE];
static Shortest_Path sp[MAX_NODE_COSUMER_SIZE];

// void initiate(char * topo[MAX_EDGE_NUM],int line_num,int &network_node,int &route,
//               int &customer_node,int &server_cost, MGraph &route_info, MGraph &customer_info)
void initiate(char *topo[MAX_EDGE_NUM], int line_num, Global_Info &g,
              Input_File_Info &network_info, Input_File_Info &customer_info)
{
    int scan = 0;
    vector<uint_16> temp(4);

    // fill in the Global infomation.
    g.line_num = line_num;
    sscanf(topo[scan++], "%d %d %d", &g.network_node, &g.links, &g.customer_node);
    sscanf(topo[++scan], "%d", &g.server_cost);
    ++scan;

    // read network node infomation.
    while(strlen(topo[++scan]) > 2)
    {
        sscanf(topo[scan], "%hd %hd %hd %hd", &temp[0], &temp[1], &temp[2], &temp[3]);
        network_info.push_back(temp);
    }

    // read costomer node infomation.
    while(++scan < line_num)
    {
        sscanf(topo[scan], "%hd %hd %hd", &temp[0], &temp[1], &temp[2]);
        customer_info.push_back(temp);
    }
}

void make_output(char *topo[MAX_EDGE_NUM])
{

}

// create the network topo: making the network nodes and the customer nodes
// in one adjacency matrix. "adjacency_matrix"
void make_adjacency_matrix(Adjacency_Matrix &adjacency_matrix, Global_Info const g,
                           Input_File_Info const network_info, Input_File_Info const customer_info)
{

    // make the undirected graph
    for (auto v : network_info)
    {
        adjacency_matrix[v[START]][v[END]] = Link_Info{(uint_8)v[BANDWIDTH], (uint_8)v[COST]};
        adjacency_matrix[v[END]][v[START]] = Link_Info{(uint_8)v[BANDWIDTH], (uint_8)v[COST]};
    }

    for (auto v : customer_info)
    {
        adjacency_matrix[v[START] + g.network_node][v[END]] = Link_Info{(uint_8)v[BANDWIDTH], 0};
        adjacency_matrix[v[END]][v[START] + g.network_node] = Link_Info{(uint_8)v[BANDWIDTH], 0};
    }
}

void print_matrix(Adjacency_Matrix const am)
{
    for (auto r : am)
    {
        for (auto l : r)
        {
            printf("%d,%d ", l.bandwidth, l.cost);
        }
        printf("\n");
    }
}

//你要完成的功能总入口
void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename)
{
	// 需要输出的内容
	char * topo_file;
    Input_File_Info network_info;
    Input_File_Info customer_info;
    Global_Info g;

    initiate(topo, line_num, g, network_info, customer_info);
    int vSize = g.network_node + g.customer_node;

    // initiate Adjacency Matrix (vSize * vSize).
    Adjacency_Matrix am(vSize, Adjacency_Matrix_Row(vSize, Link_Info{0, COST_INF}));
    make_adjacency_matrix(am, g, network_info, customer_info);
    // print_matrix(am);

    shortest_dijkstra(am, vSize, 0, path, sp);

    // for (int i = 0; i < vSize; i++)
    // {
    //     printf("path = %d, sp = %d\n", path[i], sp[i]);
    // }

    /*
    topo_file = (char *)malloc((customer_info.size()+2)*MAX_LINE_LEN);
    char route_num[20];
    sprintf(route_num, "%ld", customer_info.size());
    strncpy(topo_file, route_num, strlen(route_num));
    strncat(topo_file, "\n", 1);
    char tempchar[80];
    for(int i=0;i<customer_info.size();i++)
    {
        strncat(topo_file,"\n",1);
        sprintf(tempchar,"%d",customer_info[i][1]);
        strncat(topo_file,tempchar,strlen(tempchar));
        strncat(topo_file," ",1);
        sprintf(tempchar,"%d",customer_info[i][0]);
        strncat(topo_file,tempchar,strlen(tempchar));
        strncat(topo_file," ",1);
        sprintf(tempchar,"%d",customer_info[i][2]);
        strncat(topo_file,tempchar,strlen(tempchar));
    }
    strcat(topo_file,"\0");

    输出整数矩阵
    int size;
    size = route_info.size();
    for(i=0;i<size;i++)
    {
        for(int j=0;j<4;j++)
            cout<<route_info[i][j]<<" ";
        cout<<endl;
    }
        //cout<<"消费节点矩阵长度为"<<customer_info.size()<<endl;
    size = customer_info.size();
    for(i=0;i<size;i++)
    {
        for(int j=0;j<3;j++)
            cout<<customer_info[i][j]<<" ";
        cout<<endl;
    }
    */
    topo_file = "21312\n23423\n";
	// 直接调用输出文件的方法输出到指定文件中(ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开)
	write_result(topo_file, filename);
}
