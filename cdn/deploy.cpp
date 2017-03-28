#include "deploy.h"
#include "lib_util.h"

#include <stdio.h>
#include <vector>
#include <iostream>
#include <string.h>

using namespace std;
#define MAX_LINE_LEN 55000

/* typedef struct File_Info_t {
    int line_num;
    int network_node;
    int route;
    int customer_node;
    int server_cost;
}File_Info; */

void initiate(char * topo[MAX_EDGE_NUM],int line_num,int &network_node,int &route,
              int &customer_node,int &server_cost, MGraph &route_info, MGraph &customer_info)
{
    int scan = 0;
    int num1,num2,num3,num4;
    vector<int> temp1(4);
    vector<int> temp2(3);

    sscanf(topo[scan++],"%d %d %d",&network_node,&route,&customer_node);
    sscanf(topo[++scan],"%d",&server_cost);
    ++scan;
    while(strlen(topo[++scan])>2)
    {
        sscanf(topo[scan],"%d %d %d %d",&num1,&num2,&num3,&num4);
        temp1[0]=num1;
        temp1[1]=num2;
        temp1[2]=num3;
        temp1[3]=num4;
        route_info.push_back(temp1);
    }
    while(++scan < line_num)
    {
        sscanf(topo[scan],"%d %d %d",&num1,&num2,&num3);
        temp2[0]=num1;
        temp2[1]=num2;
        temp2[2]=num3;
        customer_info.push_back(temp2);
    }
}

void make_output(char *topo[MAX_EDGE_NUM])
{

}

//你要完成的功能总入口
void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename)
{
	// 需要输出的内容
	//char * topo_file = (char *)"17\n\n0 8 0 20\n21 8 0 20\n9 11 1 13\n21 22 2 20\n23 22 2 8\n1 3 3 11\n24 3 3 17\n27 3 3 26\n24 3 3 10\n18 17 4 11\n1 19 5 26\n1 16 6 15\n 15 13 7 13\n4 5 8 18\n2 25 9 15\n0 7 10 10\n23 24 11 23";
	char * topo_file;

    // 把topo字符串矩阵转化为几个整型数组add by zyf
    int network_node, route, customer_node;
    int server_cost;
    MGraph route_info;
    MGraph customer_info;

    initiate(topo, line_num, network_node, route, customer_node, server_cost, route_info, customer_info);





    topo_file = (char *)malloc((customer_info.size()+2)*MAX_LINE_LEN);

    char route_num[20];
    sprintf(route_num, "%d", customer_info.size());
    strncpy(topo_file,route_num,strlen(route_num));
    strncat(topo_file,"\n",1);
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

    /*输出整数矩阵
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
	// 直接调用输出文件的方法输出到指定文件中(ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开)
	write_result(topo_file, filename);

}
