#include "lib_util.h"
#include "config.h"
#include <vector>
#include <queue>
#include <stack>

static __inline void find_path(Path_Matrix &path, uint_16 path_map[], uint_16 src, uint_16 tar);
static void rebuild_matrix(Adjacency_Matrix const &am, Extern_Adjacency_Matrix &eam, Src_Tar_Set const &s, Src_Tar_Set const &t);

// shortest algorithm -- dijkstra
bool shortest_dijkstra(Extern_Adjacency_Matrix const &m, uint_16 src, uint_16 tar, Path_Matrix &path, Shortest_Path shortest_length[])
{
    uint_8 *find = new uint_8[m.size()];
    uint_16 *path_map = new uint_16[m.size()];
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
    delete[] path_map;

    return true;
}

static __inline void find_path(Path_Matrix &path, uint_16 path_map[], uint_16 src, uint_16 tar)
{
    uint_16 i = tar;
    do {
        path.insert(path.begin(), i);
        i = path_map[i];
    } while(i != src);
    path.insert(path.begin(), src);
}

// shortest algorithm -- SPFA
bool shortest_spfa(Extern_Adjacency_Matrix const &m, uint_16 src, uint_16 tar, Path_Matrix &path, Shortest_Path shortest_length[])
{
    std::queue<uint_16> q;
    uint_16 *path_map = new uint_16[m.size()];
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
    for (uint_16 dd = 0; dd < m.size(); dd++)
    {
        printf("%d:%d:%d \n", dd, path_map[dd], shortest_length[dd]);
    }

    for (auto r : m)
    {
        for (auto l : r)
        {
            // printf("%d,%d ", l.bandwidth, l.cost);
            printf("%d ", l.cost);
        }
        printf("\n");
    }
    */
    if (shortest_length[tar] == uint_16(-1))
    {
        delete[] path_map;
        delete[] in_queue;
        return false;
    }

    find_path(path, path_map, src, tar);
    delete[] path_map;
    delete[] in_queue;

    // printf("cost == %d\n", shortest_length[tar] );


    return true;
}

// tanslate adjacency matrix to extern adjacency matrix.
void am2eam(Adjacency_Matrix const &am, Extern_Adjacency_Matrix &eam)
{
    uint_16 size = am.size();

    for (uint_16 r = 0; r < size; r++)
        for (uint_16 l = 0; l < size; l++)
        {
            eam[r][l].bandwidth = am[r][l].bandwidth;
            eam[r][l].cost = am[r][l].cost;
        }
}

// rebuild extern adjacency matrix.
static void rebuild_matrix(Adjacency_Matrix const &am, Extern_Adjacency_Matrix &eam, Src_Tar_Set const &s, Src_Tar_Set const &t)
{
    uint_16 size = eam.size()-2;

    // rebuild network matrx
    for (uint_16 r = 0; r < size; r++)
    {
        for (uint_16 l = 0; l < size; l++)
        {
            eam[r][l].bandwidth = am[r][l].bandwidth;
            eam[r][l].cost = am[r][l].cost;
            eam[r][l].left = am[r][l].bandwidth;
        }
    }

    // pull into super src.
    for (uint_16 l = 0; l < s.size(); l++)
    {
        eam[size][s[l]].bandwidth = BANDWIDTH_INF;
        eam[size][s[l]].left = BANDWIDTH_INF;
        eam[size][s[l]].cost = 0;
    }

    // pull into super tar.
    for (uint_16 l = 0; l < t.size(); l++)
    {
        eam[t[l]][size+1].bandwidth = am[size+l][t[l]].bandwidth;
        eam[t[l]][size+1].left = am[size+l][t[l]].bandwidth;
        eam[t[l]][size+1].cost = 0;
    }

    /*
    for (auto r : eam)
    {
        for (auto l : r)
        {
            // printf("%d,%d ", l.bandwidth, l.cost);
            printf("%d ", l.cost);
        }
        printf("\n");
    }
    */
}

// implement the ford fulkerson from super source to super target.
void super_ford_fulkerson(Adjacency_Matrix const &am, Src_Tar_Set const &s, Src_Tar_Set const &t)
{
    uint_16 network = am.size() - t.size();
    // TODO: use dynamic(keyword: "new") instead of static defination.
    // size(eam) = network + super_src + super_tar,
    Extern_Adjacency_Matrix eam(network+2, Extern_Adjacency_Matrix_Row(network+2, Element{0, 0, 0, COST_INF}));

    // pull into super source and super customer node and rebuild extern adjacency matrix.
    rebuild_matrix(am, eam, s, t);

    // calculate the ford fulkerson from super source to super customer.
    ford_fulkerson(eam, shortest_spfa, network, network+1);

#ifdef _MY_DEBUG
    printf("okokok\n");
    for (uint_16 l = 0; l < t.size(); l++)
    {
        printf("%u->%u:%d\n", t[l], network+1, eam[t[l]][network+1].flow);
    }
#endif
}

void dfs(Extern_Adjacency_Matrix const &eam, uint_16 src, uint_16 tar)
{
    uint_32 size = eam.size() * eam.size();
    bool *visited = new uint_8[size];
    std::stack s;

    printf("%d ",src);
    visited[src]=true;
    s.push(src);

    while(!s.empty())
    {
        uint_16 i, j;
        i = s.top();

        for(j = 0; j < eam.size(); j++)
        {
            if(visited[j] == false && eam[i][j].flow !=0 && eam[i][j].cost != COST_INF)
            {
                printf("%d ",j);
                visited[j]=true;
                s.push(j);
                break;
            }
        }

        if(j == eam.size())                   
            s.pop();
    }

    delete[] visited;
}

void ford_fulkerson(Extern_Adjacency_Matrix &eam, Shortest_Func fsp, uint_16 src, uint_16 tar)
{
    // find the shortest path in weight network.
    Path_Matrix p;
    Shortest_Path *s = new Shortest_Path[eam.size()];

    // find the shortest path from "src" to "tar"
    // untill can not find a shortest path.
    int cc = 1;
    while ( fsp(eam, src, tar, p, s) )
    {
#ifdef _MY_DEBUG
        printf("fsp===%d\n", cc++);
        for (uint_16 v : p)
        {
            printf("path = %hd\n", v);
        }
#endif
        // TODO: using the build-in function min() to
        // find the min left bandwidth.
        int min = eam[p[0]][p[1]].left;
        for (uint_16 i = 1; i < p.size()-1; i++)
        {
            if (min > eam[p[i]][p[i+1]].left)
                min = eam[p[i]][p[i+1]].left;
        }

        // update the flow, left and cost.
        for (uint_16 i = 0; i < p.size()-1; i++)
        {
            // re-calculate the left bandwidth
            eam[p[i]][p[i+1]].left -= min;
            eam[p[i+1]][p[i]].left += min;

            // re-calculate the flows
            eam[p[i]][p[i+1]].flow += min;
            eam[p[i+1]][p[i]].flow -= eam[p[i]][p[i+1]].flow;

            // re-calculate the cost
            if (eam[p[i]][p[i+1]].left == 0)
                eam[p[i]][p[i+1]].cost = COST_INF;
            if (eam[p[i]][p[i+1]].flow > 0 && eam[p[i+1]][p[i]].cost != COST_INF)
                eam[p[i+1]][p[i]].cost = eam[p[i+1]][p[i]].cost < 0 ? eam[p[i+1]][p[i]].cost : -eam[p[i+1]][p[i]].cost;
            else if (eam[p[i]][p[i+1]].flow == 0)
                eam[p[i+1]][p[i]].cost = COST_INF;
        }
        p.clear();
    }

    delete[] s;
}
