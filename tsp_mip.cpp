#include <glpk.h>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <iostream>
#include <cmath>
#include <ctime>

typedef struct {
    int x, y;
    double cost;
} arc;

typedef struct {
    int posx, posy;
    int id;
} node;

double                          distance(node, node );
int                             dfs( std::vector<std::vector<double> > &solution, int n, int* visited, int *preced, int next );
int                             get_index( int i, int j, int n);
std::vector<int>                find_smallest_cycle( std::vector<std::vector<double> > &solution, int n );
std::vector<std::vector<int> >  find_all_cycles( std::vector<std::vector<double> > &solution, int n );
void                            save_svg(char *fname, std::vector<std::vector<double> > &solution, int n, std::vector<node> nodes, int scale );

int main(int argc, char *argv[]){
    std::vector<node>   nodes;
    std::vector<std::vector<double> > arcs;
    std::vector<std::vector<double> > solution;

    glp_prob *lp;
    glp_iocp parm_mip;
    glp_smcp parm_spx;

    time_t t = clock();

    double z;

    int n;
    int n_constraints;
    int non_zeros;
    int scale;
    int cycles;

    char col_name[256];

    scale = 4;
    cycles = 0;

    lp = glp_create_prob();
    glp_set_prob_name(lp, "tsp");
    glp_set_obj_dir(lp, GLP_MIN);

    glp_init_iocp(&parm_mip);

    //parm_mip.msg_lev    = GLP_MSG_OFF;      // Output level
    parm_mip.msg_lev    = GLP_MSG_ALL;      // Output level

    //parm_mip.presolve   = GLP_OFF;          //
    parm_mip.presolve   = GLP_ON;           // MIP presolver

    //parm_mip.pp_tech    = GLP_PP_NONE;      // No preprocessing
    parm_mip.pp_tech    = GLP_PP_ROOT;      // Preprocessin only on root node
    //parm_mip.pp_tech    = GLP_PP_ALL;       // Preprocessin on all nodes

    parm_mip.fp_heur    = GLP_OFF;          // Feasibility pump heuristic
    //parm_mip.fp_heur    = GLP_ON;           // Feasibility pump heuristic

    /* CUTS */
    parm_mip.gmi_cuts   = GLP_OFF;          //
    //parm_mip.gmi_cuts   = GLP_ON;           // Gomory's intiger cuts

    parm_mip.mir_cuts   = GLP_OFF;          //
    //parm_mip.mir_cuts   = GLP_ON;           // Mixed Integer Rounding Cuts

    parm_mip.cov_cuts   = GLP_ON;           // Mixed Cover Cuts
    //parm_mip.cov_cuts   = GLP_OFF;          //

    parm_mip.clq_cuts   = GLP_ON;           // Clique Cuts
    //parm_mip.clq_cuts   = GLP_OFF;         //

    /* BRANCHING */
    //parm_mip.br_tech    = GLP_BR_FFV;       // Branching method: first fractional value
    //parm_mip.br_tech    = GLP_BR_LFV;       // Branching method: least fractional value
    //parm_mip.br_tech    = GLP_BR_MFV;       // Branching method: most  fractional value
    //parm_mip.br_tech    = GLP_BR_DTH;       // Branching method: Driebeck-Tomlin heuristic
    //parm_mip.br_tech    = GLP_BR_PCH;       // Branching method: Hybrid pseudocost heuristic

    /* BACKTRACKING METHOD */
    //parm_mip.bt_tech    = GLP_BT_DFS;       // Backtracking method: Depth first search
    //parm_mip.bt_tech    = GLP_BT_BFS;       // Backtracking method: Breadth first seach
    //parm_mip.bt_tech    = GLP_BT_BLB;       // Backtracking method: Best Local Bound
    //parm_mip.bt_tech    = GLP_BT_BPH;       // Backtracking method: Best projection Heuristic


    glp_init_smcp(&parm_spx);
    parm_spx.msg_lev = GLP_MSG_ERR;

    std::cin >> n;

    /*
     * Reads all the info from stdin
     */
    for ( int i = 0 ; i < n ; i++ ) {
        node a;
        std::cin >> a.posx;
        std::cin >> a.posy;
        a.id = i+1;
        nodes.push_back(a);
    }

    /*
     * Initializes a matrix to hold up the solution
     */
    for ( int i = 0 ; i <= n ; i++ ) {
        std::vector<double> tt;
        solution.push_back(tt);
        for ( int j = 0 ; j <= n ; j++ ) {
            solution[i].push_back(0);
        }
    }

    /*
     * Computer pair-wise Euclidian distances
     */
    for ( int i = 0 ; i < n ; i++ ) {
        std::vector<double> v;
        arcs.push_back(v);

        for ( int j = 0 ; j < n ; j++ ) {
            arcs[i].push_back(distance(nodes[i], nodes[j]));
        }
    }

    /*
     * Sets up the size of the constraint matrix
     */
    //glp_add_rows(lp, 2*n); //
    glp_add_cols(lp, n*(n+1)/2); // n(n+1)/2 variables,


    /*
     * Names each column/variable and sets the x[i,j] variables bounds,
     * sets the variables to int, and sets the bounds for assigment constraints
     */
    for ( int i = 0 ; i < n ; i++ ) {
        for ( int j = i+1 ; j < n ; j++ ) {
            //std::cout << i << " " << j << " " << get_index(i, j, n) << '\n';
            int index = get_index(i, j, n);

            glp_set_col_bnds(lp, index, GLP_DB, 0.0, 1.0);

            sprintf(col_name, "x[%d,%d]", i+1, j+1);
            //printf("%d  \t->  %s\n", i*n + j + 1, col_name);

            glp_set_col_name(lp, index, col_name);
            glp_set_col_kind(lp, index, GLP_IV);
        }
    }

    for ( int i = 0 ; i < n ; i++ ) {
        int new_row = glp_add_rows(lp, 1);

        std::vector<int   > row;
        std::vector<double> val;

        row.push_back(0);
        val.push_back(0);

        for ( int j = 0 ; j < n ; j++ ) {
            if ( i == j ) continue;

            int index = get_index(i, j, n);

            //std::cout << i << " " << j << " " << index << '\n';

            row.push_back(index);
            val.push_back(1    );
        }

        glp_set_mat_row (lp, new_row, n-1, &row[0], &val[0]);
        glp_set_row_bnds(lp, new_row, GLP_FX, 2.0, 2.0);

        //std::cout << "---------\n";
    }

    /*
     * Sets the objective function coefficients
     */
    for ( int i = 0 ; i < n ; i++ ) {
        for ( int j = 0 ; j < n ; j++ ) {
            int index = get_index(i, j, n);

            glp_set_obj_coef(lp, index, arcs[i][j]);
        }
    }


    /*
     * Main loop
     */
    do {
        cycles++;

        glp_simplex(lp, &parm_spx);
        glp_intopt(lp, &parm_mip);

        z = glp_get_obj_val(lp);
        //printf("z = %f;\n", z);

        /*
         * Saves the graph so far
         */
        for ( int i = 0 ; i < n ; i++ ) {
            for ( int j = 0 ; j < n ; j++ ) {
                int index = get_index(i, j, n);

                solution[i][j] = glp_mip_col_val(lp, index);
                //printf(" %d", (int)solution[i][j]);
            }
            //printf("\n");
        }

        char fname[256];
        sprintf(fname, "w_%s_%05d.svg", argv[6], cycles);
        save_svg(fname, solution, n, nodes, scale);

        /*
         * Removes all the subtours
         */
        std::vector<std::vector<int> > all_cycles = find_all_cycles(solution, n);

        if ( all_cycles.size() == 1 ) {
            //printf("Done!!\n");
            break;
        }

        //printf("\nStep: %d \n found %d cycles\n", cycles, all_cycles.size());

        int ind   = 0;

        for ( int k = 0 ; k < all_cycles.size() ; k++ ){
            if (all_cycles[k].size() < all_cycles[ind].size()){
                ind = k;
            }
        }

        for ( int k = 0 ; k < all_cycles.size() ; k++ ){
            std::vector<int> small = all_cycles[k];

            //if ( small.size() > 6 ) continue;
            //if ( all_cycles[k].size() != all_cycles[ind].size() ) continue;

            //printf(" -> removing cycle of size %d: ", small.size() - 1);

            for ( int i = 0 ; i < small.size() ; i++ ) {
                //printf("%d ", small[i]+1);
            }
            //printf("\n");

            int new_row = glp_add_rows(lp, 1);

            std::vector<int   > row;
            std::vector<double> val;

            row.push_back(0);
            val.push_back(0);

            for ( int i = 0 ; i < small.size()-1 ; i++ ) {
                int index = get_index(small[i], small[i+1], n);

                //printf("%d %d %d %02d %d\n", i, small[i], small[i+1], index, small.size());

                row.push_back(index);
                val.push_back(1    );
            }

            glp_set_mat_row (lp, new_row, row.size()-1, &row[0], &val[0]);
            glp_set_row_bnds(lp, new_row, GLP_UP, 0.0, small.size()-2);
        }

        //if ( cycles > 2000 ) {
            //printf("Took too long! \nAborting!\n");
            //break;
        //}
    } while ( true );



    /*
     * House cleaning :)
     */
    glp_delete_prob(lp);
    glp_free_env();

    t = clock() - t;
    fprintf(stdout, "%d %f\n", n, (double)t/CLOCKS_PER_SEC);

    return EXIT_SUCCESS;

}


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

void save_svg(char *fname, std::vector<std::vector<double> > &solution, int n, std::vector<node> nodes, int scale ){
    FILE *fptr;
    fptr = fopen(fname, "wt");

    int screenx;
    int screeny;

    int minx = 1<<20;
    int miny = 1<<20;
    int maxx = 0;
    int maxy = 0;

    screenx = 800;
    screeny = 600;

    for ( int i = 0 ; i < n ; i++ ) {
        if (nodes[i].posx < minx)
            minx = nodes[i].posx;

        if (nodes[i].posy < miny)
            miny = nodes[i].posy;

        if (nodes[i].posx > maxx)
            maxx = nodes[i].posx;

        if (nodes[i].posy > maxy)
            maxy = nodes[i].posy;
    }

    //printf("%d %d %d %d\n", minx, miny, maxx, maxy);

    fprintf(fptr,"<?xml version=\"1.0\" standalone=\"no\"?>\n");
    fprintf(fptr,"<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \n");
    fprintf(fptr,"\"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n");
    fprintf(fptr,"<svg width=\"%d\%\" height=\"%d\%\" version=\"1.0\" \n", screenx, screeny);
    fprintf(fptr,"xmlns=\"http://www.w3.org/2000/svg\">\n");

    //fprintf(fptr, "<rect width=\"%d\" height=\"%d\" style=\"fill:rgb(155,155,155);stroke-width:3;stroke:rgb(155,155,155)\" />\n", screenx, screeny);

    for ( int i = 0 ; i < n ; i++ ) {
        for ( int j = 0 ; j < n ; j++ ) {
            if ( solution[i][j] == 1 ) {
                //fprintf(fptr,"<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" style=\"stroke:black;stroke-width:2\"/>\n", nodes[i].posx * scale, nodes[i].posy * scale, nodes[j].posx * scale, nodes[j].posy * scale);
            } else if (solution[i][j] > 0 && solution[i][j] < 1 ) {
                //fprintf(fptr,"<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" style=\"stroke:red;stroke-dasharray: 3 3;stroke-width:2\"/>\n", nodes[i].posx * scale, nodes[i].posy * scale, nodes[j].posx * scale, nodes[j].posy * scale);
            } else {
                //fprintf(fptr,"<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" style=\"stroke:grey;stroke-dasharray: 3 5;stroke-width:1\"/>\n", nodes[i].posx * scale, nodes[i].posy * scale, nodes[j].posx * scale, nodes[j].posy * scale);
            }
        }
    }

    for ( int i = 0 ; i < n ; i++ ) {
        for ( int j = 0 ; j < n ; j++ ) {
            if ( solution[i][j] == 1 ) {
                fprintf(fptr,"<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" style=\"stroke:black;stroke-width:2\"/>\n", (int) ((screenx * 1.0 ) * ( nodes[i].posx - minx ) / ( maxx - minx ) + ( screenx * 0.05 )),
                                                                                                                        (int) ((screeny * 1.0 ) * ( nodes[i].posy - miny ) / ( maxy - miny ) + ( screeny * 0.05 )),
                                                                                                                        (int) ((screenx * 1.0 ) * ( nodes[j].posx - minx ) / ( maxx - minx ) + ( screenx * 0.05 )),
                                                                                                                        (int) ((screeny * 1.0 ) * ( nodes[j].posy - miny ) / ( maxy - miny ) + ( screeny * 0.05 )));
            } else if (solution[i][j] > 0 && solution[i][j] < 1 ) {
                //fprintf(fptr,"<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" style=\"stroke:red;stroke-dasharray: 3 3;stroke-width:2\"/>\n", nodes[i].posx * scale, nodes[i].posy * scale, nodes[j].posx * scale, nodes[j].posy * scale);
            } else {
                //fprintf(fptr,"<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" style=\"stroke:grey;stroke-dasharray: 3 3;stroke-width:1\"/>\n", nodes[i].posx * scale, nodes[i].posy * scale, nodes[j].posx * scale, nodes[j].posy * scale);
            }
        }
    }

    for ( int i = 0 ; i < n ; i++ ){
        fprintf(fptr,"<circle cx=\"%d\" cy=\"%d\" r=\"5\" stroke=\"black\" stroke-width=\"2\" fill=\"red\"/>\n", (int) ((screenx * 1.0 ) * ( nodes[i].posx - minx ) / ( maxx - minx ) + ( screenx * 0.05 )),
                                                                                                                 (int) ((screeny * 1.0 ) * ( nodes[i].posy - miny ) / ( maxy - miny ) + ( screeny * 0.05 )));
        //fprintf(fptr,"<text x=\"%d\" y=\"%d\" fill=\"black\">%d</text>", (nodes[i].posx * scale) + 5 , (nodes[i].posy * scale)-2 , i+1);
    }

    fprintf(fptr,"</svg>\n");

    fclose(fptr);
}


