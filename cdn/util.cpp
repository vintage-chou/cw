#include "lib_util.h"
#include "config.h"
#include <vector>
#include <queue>
#include <stack>

bool find_path(Path_Matrix &path, Path_Matrix const &path_map, uint_16 src, uint_16 tar);
void calc_cost(Extern_Adjacency_Matrix const &eam, std::vector<uint_16> const &r, uint_16 &flow, uint_16 &cost);
void rebuild_matrix(Adjacency_Matrix const &am, Extern_Adjacency_Matrix &eam, Src_Tar_Set const &s, Src_Tar_Set const &t);
void list_path(Extern_Adjacency_Matrix const &eam, Output_File_Info &of, uint_16 src, uint_16 tar);

// shortest algorithm -- dijkstra
bool shortest_dijkstra(Extern_Adjacency_Matrix const &m, uint_16 src, uint_16 tar, Path_Matrix &path, Shortest_Path shortest_length[])
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
        if (circl[i]) return false;
    } while(i != src);
    path.insert(path.begin(), src);

    return true;
}

// shortest algorithm -- SPFA
bool shortest_spfa(Extern_Adjacency_Matrix const &m, uint_16 src, uint_16 tar, Path_Matrix &path, Shortest_Path shortest_length[])
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
void rebuild_matrix(Adjacency_Matrix const &am, Extern_Adjacency_Matrix &eam, Src_Tar_Set const &s, Src_Tar_Set const &t)
{
    uint_16 size = eam.size()-2;
    printf("am.size()=%d, eam.size()=%d\n",am.size(),eam.size());
    printf("am[0].size()=%d, eam[0].size()=%d\n",am[0].size(),eam[0].size());
    return;
    printf("rebuilding...\n");
    // rebuild network matrx
    /*for (uint_16 r = 0; r < size; r++)
    {
        for (uint_16 l = 0; l < size; l++)
        {
            eam[r][l].bandwidth = am[r][l].bandwidth;
            eam[r][l].cost = am[r][l].cost;
            //eam[r][l].origin_cost = am[r][l].cost;
            eam[r][l].left = am[r][l].bandwidth;
        }
    }
    printf("rebuilding...2\n");
    // pull into super src.
    for (uint_16 l = 0; l < s.size(); l++)
    {
        eam[size][s[l]].bandwidth = BANDWIDTH_INF;
        eam[size][s[l]].left = BANDWIDTH_INF;
        eam[size][s[l]].cost = 0;
        // eam[size][s[l]].origin_cost = 0;
    }
    printf("rebuilding...3\n");
    // pull into super tar.
    for (uint_16 l = 0; l < t.size(); l++)
    {
        eam[t[l]][size+1].bandwidth = am[size+l][t[l]].bandwidth;
        eam[t[l]][size+1].left = am[size+l][t[l]].bandwidth;
        eam[t[l]][size+1].cost = 0;
        // eam[t[l]][size+1].origin_cost = 0;
    }

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
    Extern_Adjacency_Matrix *Peam = new Extern_Adjacency_Matrix(network+2, Extern_Adjacency_Matrix_Row(network+2, Element{0, 0, 0, COST_INF}));
    Extern_Adjacency_Matrix eam = *Peam;
    // pull into super source and super customer node and rebuild extern adjacency matrix.
    rebuild_matrix(am, eam, s, t);

    printf("ok");
    // calculate the ford fulkerson from super source to super customer.
    ford_fulkerson(eam, shortest_spfa, network, network+1);

    printf("okokok\n");

    Output_File_Info of;

    list_path(eam, of, network, network+1);

    for (auto r : of)
    {
        for (auto l : r)
        {
            printf("%d ", l);
        }
        puts("");
    }
    delete[] Peam;
}

void calc_cost(Extern_Adjacency_Matrix const &eam, std::vector<uint_16> const &r, uint_16 &flow, uint_16 &cost)
{
    uint_16 size = eam.size();

    cost = 0;
    flow = eam[r[size-2]][r[size-1]].flow;
    for (uint_16 i = 0; i < r.size()-1; i++)
    {
        if (flow > eam[r[i]][r[i+1]].flow)
            flow = eam[r[i]][r[i+1]].flow;
        cost += eam[r[i]][r[i+1]].cost;
    }
    cost *= flow;
}

void list_path(Extern_Adjacency_Matrix const &eam, Output_File_Info &of, uint_16 src, uint_16 tar)
{
    std::vector<uint_16> row;
    uint_16 i, j = 0;
    bool *is_in = new bool[eam.size()];

    for (uint_16 ii = 0; ii < eam.size(); ii ++)
        is_in[ii] = false;
    row.push_back(src);
    is_in[src] = true;

    while (!row.empty())
    {
        i = row.back();
        is_in[i] = true;

        for (; j < eam.size(); j++)
        {
            printf("%d->%d::%d\n", i, j, eam[i][j].flow);
            if (eam[i][j].flow > 0)
            {
                printf("push:%d\n", j);
                row.push_back(j);
                if (j == tar)
                {
                    uint_16 flow, cost;
                    calc_cost(eam, row, flow, cost);
                    row.push_back(flow);
                    row.push_back(cost);


                    for (auto l : row)
                    {
                        printf("%d ", l);
                    }
                    printf("\n");
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

        if (j == eam.size())
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

void ford_fulkerson(Extern_Adjacency_Matrix &eam, Shortest_Func fsp, uint_16 src, uint_16 tar)
{
    // find the shortest path in weight network.
    Path_Matrix p;
    Shortest_Path *s = new Shortest_Path[eam.size()];
    printf("ford_fulkerson...");
    // find the shortest path from "src" to "tar"
    // untill can not find a shortest path.
    while ( fsp(eam, src, tar, p, s) )
    {
#ifdef _MY_DEBUG
        for (uint_16 v : p)
        {
            printf("%hd->", v);
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
            eam[p[i+1]][p[i]].flow -= min;

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
