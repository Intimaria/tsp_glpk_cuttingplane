#ifndef GRAPH_H
#define GRAPH_H

#include <vector>

#include "types.h"

double                         distance( node, node );
int                            dfs( std::vector<std::vector<double> > &solution, int n, int* visited, int *preced, int next );
int                            get_index( int i, int j, int n);
std::vector<int>               find_smallest_cycle( std::vector<std::vector<double> > &solution, int n );
std::vector<std::vector<int> > find_all_cycles( std::vector<std::vector<double> > &solution, int n );

#endif /* GRAPH_H */
