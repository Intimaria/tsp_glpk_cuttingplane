#include <vector>
#include <cmath>
#include <cstdlib>

#include "types.h"
#include "graph.h"

/*
 * Helper function to compute Euclidian distance
 */
double distance(node a, node b){
    return sqrt(pow(a.posx - b.posx, 2) + pow(a.posy - b.posy, 2));
}


/*
 * Help function that returns the converted index to an edge, if the vertex is called on the first time then allocate a name for it
 */
int get_index( int i, int j, int n){
    static std::vector<std::vector<int> > adj;
    static bool init = true;
    static int  next = 1;

    if ( init ) {
        for (int i = 0 ; i < n ; i++ ) {
            std::vector<int> v;
            adj.push_back(v);

            for (int j = 0 ; j < n ; j++ ) {
                adj[i].push_back(-1);
            }
        }

        init = false;
    }

    if ( i < j ) {
        if ( adj[i][j] == -1 ) {
            adj[i][j] = next++;
        }
        return adj[i][j];
    } else {
        if ( adj[j][i] == -1 ) {
            adj[j][i] = next++;
        }
        return adj[j][i];
    }
}

std::vector<std::vector<int> > find_all_cycles( std::vector<std::vector<double> > &solution, int n ) {
    std::vector<std::vector<int> > all_cycles;

    int t_size;

    int *visited = (int*) malloc (sizeof(int) * n);
    int *preced  = (int*) malloc (sizeof(int) * n);

    for ( int i = 0 ; i < n ; i++ ) {
        visited[i] = 0;
        preced[i]  = 0;
    }

    for ( int i = 0 ; i < n ; i++ ) {
        if ( visited[i] == 1 ) {
            continue;
        } else {
            t_size = dfs(solution, n, visited, preced, i);

            std::vector<int> tt;
            int t = i;
            tt.push_back(t);

            do {
                t = preced[t];
                tt.push_back(t);
            } while ( t != i );

            all_cycles.push_back(tt);
        }
    }

    free(visited);
    free(preced );

    return all_cycles;
}

int dfs( std::vector<std::vector<double> > &solution, int n, int* visited, int *preced, int next ) {
    if (visited[next] == 1 ) {
        return 0;
    } else {
        for ( int i = 0 ; i < n ; i++ ) {
            if ( i == preced[next] ) continue;
            if ( solution[next][i] == 1 ) {
                visited[next] = 1;
                preced[i] = next;
                return 1 + dfs(solution, n, visited, preced, i);
            }
        }
    }
}




