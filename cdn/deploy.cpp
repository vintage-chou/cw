#include "deploy.h"
#include "lib_util.h"
#include "config.h"

#include <stdio.h>
#include <vector>
#include <set>
#include <iostream>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include <string>
#include <sstream>
#include "immune.h"

#define MAX_LINE_LEN 55000

using namespace std;

typedef struct Global_Info_t {
    int line_num;
    int network_node;
    int links;
    int customer_node;
    int server_cost;
    uint_32 customer_need;
    Src_Tar_Set src;
    Src_Tar_Set tar;
}Global_Info;

// typedef vector <uint_32> Knowledge_Cap;
typedef uint_32 Knowledge_Cap;

uint_32 make_output(Output_File_Info &of, Feedback &fb, string &out, Global_Info const &g);
string toString(uint_16 num);
void create_init_src(Global_Info const &g, Input_File_Info const &network_info,
					 Input_File_Info const &customer_info, Output_File_Info &of);
void make_adjacency_matrix(Adjacency_Matrix &am, Global_Info const &g,
                           Input_File_Info const network_info, Input_File_Info const customer_info);
void check_srcs(Global_Info const &g, Input_File_Info const &network_info,
                Input_File_Info const &customer_info, Output_File_Info &of);
void my_create_gene(vector<T> &igg);

// TODO: use map instead !!
static uint_16 customer_need[MAX_NODE_SIZE];
static uint_16 customer_local[MAX_NODE_SIZE];

//double start_time,current_time;
struct timeval s_time, e_time;

void my_create_gene(vector<T> &igg, Global_Info const &g)
{
	g.tar;
}

void create_init_src(Global_Info const &g, Input_File_Info const &network_info,
					 Input_File_Info const &customer_info, Output_File_Info &of)
{
	int vSize = g.network_node + 2;
	int size = vSize - 2;
	uint_16 d_cost = 1000;

	// initiate Adjacency Matrix (vSize * vSize).
	Adjacency_Matrix am(vSize, Adjacency_Matrix_Row(vSize, Element{0, 0, 0, COST_INF, COST_INF}));

	// make the undirected graph
	for (auto v : network_info)
	{
	   am[v[END]][v[START]] = am[v[START]][v[END]] =  \
	   Element{(uint_16)v[BANDWIDTH], 0, (uint_16)v[BANDWIDTH], (char)v[COST], (char)v[COST]};
	}

	// pull into super src.
	for (uint_16 l = 0; l < g.tar.size(); l++)
	{
	   am[size][g.tar[l]].bandwidth = customer_need[g.tar[l]];
	   am[size][g.tar[l]].left = customer_need[g.tar[l]];
	   am[size][g.tar[l]].cost = 0;
	   am[size][g.tar[l]].origin_cost = 0;
	}

	Src_Tar_Set src;
	for (uint_16 l = 0; l < g.network_node; l++)
	{
		if ( find(g.tar.begin(), g.tar.end(), l) == g.tar.end())
		{
			src.push_back(l);
		}
	}

	// pull into super tar.
	for (uint_16 l = 0; l < src.size(); l++)
	{
	   am[src[l]][size+1].bandwidth = BANDWIDTH_INF;
	   am[src[l]][size+1].left = BANDWIDTH_INF;
	   am[src[l]][size+1].cost = d_cost;
	   am[src[l]][size+1].origin_cost = d_cost;
	}

	super_ford_fulkerson(am, of, shortest_spfa2);

}

void initiate(char *topo[MAX_EDGE_NUM], int line_num, Global_Info &g,
              Input_File_Info &network_info, Input_File_Info &customer_info,
			  Knowledge_Cap *kc)
{
    int scan = 0;
    vector<uint_16> temp(4);

    // fill in the Global infomation.
    g.line_num = line_num;
    g.customer_need = 0;
    sscanf(topo[scan++], "%d %d %d", &g.network_node, &g.links, &g.customer_node);
    sscanf(topo[++scan], "%d", &g.server_cost);
    ++scan;

    // read network node infomation.
    while(strlen(topo[++scan]) > 2)
    {
        sscanf(topo[scan], "%hd %hd %hd %hd", &temp[0], &temp[1], &temp[2], &temp[3]);
        network_info.push_back(temp);
        kc[temp[0]] += temp[2];
        kc[temp[1]] += temp[2];
    }

    // read costomer node infomation.
    while(++scan < line_num)
    {
        sscanf(topo[scan], "%hd %hd %hd", &temp[0], &temp[1], &temp[2]);
        g.tar.push_back((uint_16)temp[1]);
        g.customer_need += temp[2];
        customer_need[temp[1]] = temp[2];
        customer_local[temp[1]] = temp[0];
        customer_info.push_back(temp);
    }
}

// create the network topo: making the network nodes and the customer nodes
// in one adjacency matrix. "adjacency_matrix"
void make_adjacency_matrix(Adjacency_Matrix &am, Global_Info const &g,
                           Input_File_Info const network_info, Input_File_Info const customer_info)
{
    int size = am.size()-2;

    // make the undirected graph
    for (auto v : network_info)
    {
        am[v[END]][v[START]] = am[v[START]][v[END]] =  \
        Element{(uint_16)v[BANDWIDTH], 0, (uint_16)v[BANDWIDTH], (char)v[COST], (char)v[COST]};
    }

    // pull into super src.
    for (uint_16 l = 0; l < g.src.size(); l++)
    {
        am[size][g.src[l]].bandwidth = BANDWIDTH_INF;
        am[size][g.src[l]].left = BANDWIDTH_INF;
        am[size][g.src[l]].cost = 0;
        am[size][g.src[l]].origin_cost = 0;
    }

    // pull into super tar.
    for (uint_16 l = 0; l < g.tar.size(); l++)
    {
        am[g.tar[l]][size+1].bandwidth = customer_need[g.tar[l]];
        am[g.tar[l]][size+1].left = customer_need[g.tar[l]];
        am[g.tar[l]][size+1].cost = 0;
        am[g.tar[l]][size+1].origin_cost = 0;
    }
}

string toString(uint_16 num)
{
    stringstream ss; string tmp;
    ss << num; ss >> tmp;
    return  tmp;
}

// Output_File_Info_Row:
// 0     1    2   ...   n-4   n-3   n-2   n-1
// s_src src  mid  ...  tar   s_tar flow  cost
uint_32 make_output(Output_File_Info &of, Feedback &fb, string &out, Global_Info const &g)
{
    uint_16 links = of.size();
    uint_16 i = 1;
    uint_64 cost = 0;
    set<uint_16> node;
    set<uint_16>::iterator it;
    uint_32 flow_map[MAX_NODE_SIZE] = {0};

    vector<short> fbr;

    out = out + toString(links) + "\n\n";

    for (Output_File_Info_Row r : of)
    {
        uint_16 size = r.size();
        for (i = 1; i < size-3; i++)
            out = out + toString(r[i]) + " ";

        out = out + toString(customer_local[r[size-4]]) + " " + toString(r[size-2]) + "\n";
        cost += r[size-1];
        flow_map[r[size-4]] += r[size-2];
        node.insert(r[size-4]);
    }

    // get more detils
    for (it = node.begin(); it != node.end(); it++)
    {
#if DETIL_FEEDBACK
        fbr.push_back(*it);
        fbr.push_back(flow_map[*it]);
        fbr.push_back(flow_map[*it] - customer_need[*it]);
        fb.push_back(fbr);
        fbr.clear();
#endif
        fbr.push_back(flow_map[*it]);
    }

#if DETIL_FEEDBACK == 0
    int sum = 0;
    uint_16 k = 0;
    for (k = 0; k < fbr.size(); k++)
    {
        sum += fbr[k];
    }

    vector<uint_32> r;
    // (sum < 0) || (k < g.customer_node) ? r.push_back(0) : r.push_back(1);

    r.push_back(cost);
    r.push_back(g.customer_need-sum);

    fb.push_back(r);
#endif

    out[out.length()-1] = '\0';

    return cost;
}

static __inline void print_feedback(Feedback &fb)
{
    for (auto r : fb)
    {
        for (short l : r)
            printf("%d ", l);
        puts("");
    }
}

void check_srcs(Global_Info const &g, Input_File_Info const &network_info,
                Input_File_Info const &customer_info, Output_File_Info &of)
{
    int vSize = g.network_node + 2;

    // initiate Adjacency Matrix (vSize * vSize).
    Adjacency_Matrix am(vSize, Adjacency_Matrix_Row(vSize, Element{0, 0, 0, COST_INF, COST_INF}));
    make_adjacency_matrix(am, g, network_info, customer_info);
    // print_matrix(am);

    super_ford_fulkerson(am, of, shortest_spfa);

    // print_matrix(am);
}

void create_one_srcs(Global_Info &g, vector<uint_16> r)
{
    g.src.clear();

    for (uint_16 i = 0; i < r.size(); i++)
    {
       if (r[i])
       {
    	   g.src.push_back(i);
       }
    }
}

void deploy_server(char * topo[MAX_EDGE_NUM], int line_num, char * filename)
{
    gettimeofday(&s_time, NULL);
    Input_File_Info network_info;
    Input_File_Info customer_info;
    Global_Info g;

    Output_File_Info of;
    string out;
    string final_out;
    Feedback fb;
    uint_32 link_cost, server_cost, total_cost, min_total_cost = uint_32(-1);

    Knowledge_Cap knowledge_cap[MAX_NODE_SIZE] = {0};

    initiate(topo, line_num, g, network_info, customer_info, knowledge_cap);

    vector<uint_32> knowledge_capacity(knowledge_cap, knowledge_cap+g.network_node);
    create_init_src(g, network_info, customer_info, of);

#if 1
    immune teach(customer_info, g.network_node, g.server_cost, knowledge_capacity, g.customer_need);
    teach.imCreateGene();

    my_create_gene(vector<T> teach.im_Gen_Gene);

    gettimeofday(&e_time, NULL);

//    Src_Tar_Set ss;
//
//
//    ss.push_back(6);
//    ss.push_back(7);
//    ss.push_back(13);
//    ss.push_back(17);
//    ss.push_back(35);
//    ss.push_back(41);
//    ss.push_back(48);
//
//    for (int i = 0; i < teach.im_Gen_Gene[2].size(); i++) {
//		if (find(ss.begin(), ss.end(), i) != ss.end())
//		{
//			teach.im_Gen_Gene[2][i] = 1;
//		}
//		else
//		{
//			teach.im_Gen_Gene[2][i] = 0;
//		}
//    }

	while ((e_time.tv_sec-s_time.tv_sec) < RUNNING_TIME)
	{
		//send to 最小费用最大流
		// 6, 7, 13, 17, 35, 41, 48
		for ( vector<uint_16> r : teach.im_Gen_Gene)
		{

			create_one_srcs(g, r);
			check_srcs(g, network_info, customer_info, of);
			server_cost = g.server_cost * g.src.size();
			link_cost = make_output(of, fb, out, g);
			fb[fb.size()-1][0] += server_cost;
			total_cost = server_cost + link_cost;
			if (fb[fb.size()-1][1] == 0 && min_total_cost > total_cost)
			{
				min_total_cost = total_cost;
				final_out = out;
				// cout << out << endl;

				printf("count = %lu\n", g.src.size());
				printf("Total Cost = %lu\n", total_cost);
			}

			of.clear();
		}

		//训练基因
		printf("----------------\n");
		print_feedback(fb);

		//printf("MAIN_IMGENGEN_BEGIN\n");
		//teach.imPrintInfo(teach.im_Gen_Gene);

		teach.imAfinity(fb);
		teach.imDensity();
		teach.imBreedProbability();
		teach.imGeneChoose();
		teach.imGeneMix();
	    	//teach.imPrintInfo(teach.im_Gen_Gene_Father);
		teach.imGeneAberrance(fb);
	    	//teach.imPrintInfo(teach.im_Gen_Gene);
		fb.clear();
		gettimeofday(&e_time, NULL);

	}
#endif
    // make output file.
    char *topo_file;
    uint_32 len = final_out.length();
    topo_file = (char *)malloc((len+1)*sizeof(char));
    memset(topo_file, 0, len+1);
    final_out.copy(topo_file, len, 0);
	write_result(topo_file, filename);
	printf("Final Total Cost = %lu\n", min_total_cost);

    free(topo_file);

    #ifdef _MY_DEBUG
        printf("targetSize = %u\n", g.tar.size());
        for (auto v : g.tar)
        {
            printf("%u\t", v);
        }
        puts("");

        Path_Matrix path;
        Shortest_Path sp[MAX_NODE_SIZE];
        printf("size##=%d\n", vSize);

        shortest_dijkstra(am, 2, 4, path, sp);

        printf("dijkstra\n");
        for (uint_16 v : path)
        {
            printf("%hd->", v);
        }
        printf("cost = %d\n", sp[4]);

        path.clear();
        memset(sp, COST_INF, MAX_NODE_SIZE);
        shortest_spfa(am, 2, 4, path, sp);
        printf("SPFA\n");
        for (uint_16 v : path)
        {
            printf("%hd->", v);
        }
        printf("cost = %d\n", sp[4]);
    #endif
}
