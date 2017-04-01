#include "lib_util.h"
#include "config.h"
#include <vector>
#include <queue>
#include <stack>

bool find_path(Path_Matrix &path, Path_Matrix const &path_map, uint_16 src, uint_16 tar);
void calc_cost(Adjacency_Matrix const &am, Path_Matrix const &p, uint_16 &flow, uint_16 &cost);
void list_path(Adjacency_Matrix const &am, Output_File_Info &of, uint_16 src, uint_16 tar);

// shortest algorithm -- dijkstra
bool shortest_dijkstra(Adjacency_Matrix const &m, uint_16 src, uint_16 tar, Path_Matrix &path, Shortest_Path shortest_length[])
{
    uint_8 *find = new uint_8[m.size()];
    Path_Matrix path_map(m.size());
    uint_16 v, w, k = src;

    for (v = 0; v < m.size(); v++)
    {
        shortest_length[v] = m[src][v].cost;
        path_map[v] = src;
        find[v] = 0;
    }

    shortest_length[src] = 0;
    find[src] = 1;
    for (v = 1; v < m.size(); v++)
    {
        uint_16 min = COST_INF;
        for (w = 0; w < m.size(); w++)
        {
            if ( !find[w] && min > shortest_length[w] )
            {
                k = w;
                min = shortest_length[w];
            }
        }
        // printf("%d:min(%d)=%d\n",v, k, shortest_length[k]);
        find[k] = 1;

        for (w = 0; w < m.size(); w++)
        {
            if ( !find[w] && min + m[k][w].cost < shortest_length[w] )
            {
                shortest_length[w] = min + m[k][w].cost;
                path_map[w] = k;
            }
        }
    }

    /* for (int x = 0; x < m.size(); x++)
    {
        printf("%d:%d ", x, path_map[x]);
    }*/

    find_path(path, path_map, src, tar);

    delete[] find;

    return true;
}

bool find_path(Path_Matrix &path, Path_Matrix const &path_map, uint_16 src, uint_16 tar)
{
    uint_16 i = tar;
    std::vector<bool> circl(path_map.size(), false);

    do {
        circl[i] = true;
        path.insert(path.begin(), i);
        i = path_map[i];
        if (circl[i] || i == NO_PARENT) return false;
    } while(i != src);
    path.insert(path.begin(), src);

    return true;
}

// shortest algorithm -- SPFA
bool shortest_spfa(Adjacency_Matrix const &m, uint_16 src, uint_16 tar, Path_Matrix &path, Shortest_Path shortest_length[])
{
    std::queue<uint_16> q;
    Path_Matrix path_map(m.size());
    bool *in_queue = new bool[m.size()];
    uint_16 k = src;

    for (uint_16 v = 0; v < m.size(); v++)
    {
        in_queue[v] = false;
        shortest_length[v] = uint_16(-1);
        path_map[v] = NO_PARENT;
    }
    q.push(src);
    shortest_length[src] = 0;
    in_queue[src] = true;
    path_map[src] = src;

    while(!q.empty())
    {
        k = q.front();
        q.pop();
        in_queue[k] = false;
        // relax
        for (uint_16 v = 0; v < m.size(); v++)
        {
            if(m[k][v].cost == COST_INF || shortest_length[k] == uint_16(-1))
                continue;

            if (shortest_length[v] > shortest_length[k] + m[k][v].cost)
            {
                shortest_length[v] = shortest_length[k] + m[k][v].cost;
                path_map[v] = k;
                // if v isn`t in the queue.
                if (! in_queue[v])
                {
                    q.push(v);
                    in_queue[v] = true;
                }
            }
        }
        // end relax
    }
/*
#ifdef _MY_DEBUG
    for (uint_16 dd = 0; dd < m.size(); dd++)
    {
        printf("%d:%d:%d \n", dd, path_map[dd], shortest_length[dd]);
    }

    for (auto r : m)
    {
        for (auto l : r)
        {
            // printf("%d,%d ", l.bandwidth, l.cost);
            printf("%d ", l.flow);
        }
        printf("\n");
    }
#endif
*/
    // if (shortest_length[tar] == uint_16(-1))
    // {
    //     delete[] path_map;
    //     delete[] in_queue;
    //     return false;
    // }

    delete[] in_queue;

    return find_path(path, path_map, src, tar);
}

// implement the ford fulkerson from super source to super target.
void super_ford_fulkerson(Adjacency_Matrix &am, Output_File_Info &of)
{
    uint_16 network = am.size()-2;

    // calculate the ford fulkerson from super source to super customer.
    ford_fulkerson(am, shortest_spfa, network, network+1);

    // make the list of path.
    // super_src -> mid_node_1 -> mid_node_2 -> ... -> mid_node_n -> flow -> cost
    list_path(am, of, network, network+1);
}

// TODO: combine the function "calc_cost" and function "list_path" in one function
void calc_cost(Adjacency_Matrix const &am, Path_Matrix const &p, uint_16 &flow, uint_16 &cost)
{
    uint_16 size = p.size()-1;

    cost = 0;
    flow = am[p[size-1]][p[size]].flow;
    for (uint_16 i = 0; i < size; i++)
    {
        if (flow > am[p[i]][p[i+1]].flow)
            flow = am[p[i]][p[i+1]].flow;
        cost += am[p[i]][p[i+1]].origin_cost;
    }
    cost *= flow;
}

// TODO:
void list_path(Adjacency_Matrix const &am, Output_File_Info &of, uint_16 src, uint_16 tar)
{
    std::vector<uint_16> row;
    uint_16 i, j = 0;
    bool *is_in = new bool[am.size()];

    for (uint_16 ii = 0; ii < am.size(); ii ++)
        is_in[ii] = false;
    row.push_back(src);
    is_in[src] = true;

    while (!row.empty())
    {
        i = row.back();
        is_in[i] = true;

        for (; j < am.size(); j++)
        {
            // printf("%d->%d::%d\n", i, j, am[i][j].flow);
            if (am[i][j].flow > 0)
            {
                // printf("push:%d\n", j);
                row.push_back(j);
                if (j == tar)
                {
                    uint_16 flow, cost;
                    calc_cost(am, row, flow, cost);
                    row.push_back(flow);
                    row.push_back(cost);
                    of.push_back(row);
                    row.erase(row.end()-3, row.end());
                    j = row.back();
                    row.pop_back();
                    i = row.back();
                }
                else
                    break;
            }
        }

        if (j == am.size())
        {
            j = row.back()+1;
            row.pop_back();
            // printf("flag = %d\n", row.back());
        }
        else
        {
            if (!is_in[row.back()]) j = 0;
        }
    }
    delete[] is_in;
}

void ford_fulkerson(Adjacency_Matrix &am, Shortest_Func fsp, uint_16 src, uint_16 tar)
{
    // find the shortest path in weight network.
    Path_Matrix p;
    Shortest_Path *s = new Shortest_Path[am.size()];
    // printf("ford_fulkerson...");

    // find the shortest path from "src" to "tar"
    // untill can not find a shortest path.
    while ( fsp(am, src, tar, p, s) )
    {
        // print_matrix(am);
        // for (uint_16 v : p)
        // {
        //     printf("%hd->", v);
        // }
        // puts("");
        // TODO: using the build-in function min() to
        // find the min left bandwidth.
        int min = am[p[0]][p[1]].left;
        for (uint_16 i = 1; i < p.size()-1; i++)
        {
            if (min > am[p[i]][p[i+1]].left)
                min = am[p[i]][p[i+1]].left;
        }

        // update the flow, left and cost.
        for (uint_16 i = 0; i < p.size()-1; i++)
        {
            // re-calculate the left bandwidth
            am[p[i]][p[i+1]].left -= min;
            am[p[i+1]][p[i]].left += min;

            // re-calculate the flows
            am[p[i]][p[i+1]].flow += min;
            am[p[i+1]][p[i]].flow -= min;

            // re-calculate the cost
            if (am[p[i]][p[i+1]].left == 0)
                am[p[i]][p[i+1]].cost = COST_INF;
            if (am[p[i]][p[i+1]].flow > 0 && am[p[i+1]][p[i]].cost != COST_INF)
                am[p[i+1]][p[i]].cost = am[p[i+1]][p[i]].cost < 0 ? am[p[i+1]][p[i]].cost : -am[p[i+1]][p[i]].cost;
            else if (am[p[i]][p[i+1]].flow == 0)
                am[p[i+1]][p[i]].cost = COST_INF;
        }
        p.clear();
    }

    delete[] s;
}

void print_matrix(Adjacency_Matrix const &am)
{
    for (auto p : am)
    {
        for (auto l : p)
        {
            // printf("%d,%d ", l.bandwidth, l.cost);
            printf("%d ", l.origin_cost);
        }
        printf("\n");
    }
}
