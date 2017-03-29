#include "lib_util.h"
#include "config.h"

void shortest_dijkstra(Adjacency_Matrix m, int size, int v0, Path_Matrix path_matrix[], Shortest_Path shortest_length[])
{
    int final[MAX_NODE_SIZE];
    int v, w, k = v0;

    printf("size=%d\n", size);
    for (v = 0; v < size; v++)
    {
        shortest_length[v] = m[v0][v].cost;
        printf("init:%d->%d\n", v, shortest_length[v]);
        path_matrix[v] = 0;
        final[v] = 0;
    }

    shortest_length[v0] = 0;
    final[v0] = 1;
    char f = 1;
    printf("shortest_length[1] = %d\n", shortest_length[1]);
    for (v = 1; v < size; v++)
    {
        int min = COST_INF;
        for (w = 0; w < size; w++)
        {
            if ( !final[w] && min > shortest_length[w] )
            {
                k = w;
                min = shortest_length[w];
                if (f)
                {
                    printf("shortest_length[%d] = %d\n", w, shortest_length[w]);
                }
            }
        }
        f = 0;
        // printf("%d:min(%d)=%d\n",v, k, min);
        final[k] = 1;

        for (w = 0; w < size; w++)
        {
            if ( !final[w] && min + m[k][w].cost < shortest_length[w] )
            {
                shortest_length[w] = min + m[k][w].cost;
                path_matrix[w] = k;
            }
        }
    }
}
