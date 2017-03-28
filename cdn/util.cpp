#include "lib_util.h"
#include "config"

#define INF 65535

int shortest_dijkstra(MGraph m, int v0, int path_matrix[], int shortest_length[])
{
    int final[MAX_NODE_SIZE];
    int v, k, j = 0;

    for (v = 0; v < m.max_size; v++)
    {
        shortest_length[v] = INF;
        final[v] = 0;
    }

    shortest_length[v0] = 0;
    final[v0] = 1;

    for (v = 1; v < m.max_size; v++)
    {
        min = INF;
        for (w = 0; w < m.max_size; w++)
        {
            if ( !final[w] && min > shortest_length[w] )
            {
                k = w;
                min = shortest_length[w];
            }
        }
        final[k] = 1;

        for (w = 0; w < m.max_size; w++)
        {
            if ( !final[w] && min + m.matrix[k][w] < shortest_length[w] )
            {
                shortest_length[w] = min + m.matrix[k][w];
                path_matrix[w] = k;
            }            
        }
    }
}
