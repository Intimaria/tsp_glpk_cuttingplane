#include <cstdlib>
#include <cstdio>
#include <vector>
#include <iostream>
#include <cmath>
#include <ctime>

#include <glpk.h>

#include "types.h"
#include "graph.h"
#include "svg_output.h"

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
    //int n_constraints;
    //int non_zeros;
    int scale;
    int cycles;

    char col_name[256];

    scale  = 4;
    cycles = 0;

    lp = glp_create_prob();
    glp_set_prob_name(lp, "tsp");
    glp_set_obj_dir(lp, GLP_MIN);

    glp_init_iocp(&parm_mip);

    parm_mip.msg_lev    = GLP_MSG_OFF;      // Output level
    //parm_mip.msg_lev    = GLP_MSG_ALL;      // Output level

    //parm_mip.presolve   = GLP_OFF;          //
    parm_mip.presolve   = GLP_ON;           // MIP presolver

    //parm_mip.pp_tech    = GLP_PP_NONE;      // No preprocessing
    parm_mip.pp_tech    = GLP_PP_ROOT;      // Preprocessin only on root node
    //parm_mip.pp_tech    = GLP_PP_ALL;       // Preprocessin on all nodes

    parm_mip.fp_heur    = GLP_OFF;          // Feasibility pump heuristic
    //parm_mip.fp_heur    = GLP_ON;           // Feasibility pump heuristic

    /* CUTS */
    //parm_mip.gmi_cuts   = GLP_OFF;          //
    parm_mip.gmi_cuts   = GLP_ON;           // Gomory's intiger cuts

    parm_mip.mir_cuts   = GLP_OFF;          //
    //parm_mip.mir_cuts   = GLP_ON;           // Mixed Integer Rounding Cuts

    parm_mip.cov_cuts   = GLP_OFF;          //
    //parm_mip.cov_cuts   = GLP_ON;           // Mixed Cover Cuts

    parm_mip.clq_cuts   = GLP_OFF;         //
    //parm_mip.clq_cuts   = GLP_ON;           // Clique Cuts

    /* BRANCHING */
    //parm_mip.br_tech    = GLP_BR_FFV;       // Branching method: first fractional value
    //parm_mip.br_tech    = GLP_BR_LFV;       // Branching method: least fractional value
    //parm_mip.br_tech    = GLP_BR_MFV;       // Branching method: most  fractional value
    parm_mip.br_tech    = GLP_BR_DTH;       // Branching method: Driebeck-Tomlin heuristic
    //parm_mip.br_tech    = GLP_BR_PCH;       // Branching method: Hybrid pseudocost heuristic

    /* BACKTRACKING METHOD */
    //parm_mip.bt_tech    = GLP_BT_DFS;       // Backtracking method: Depth first search
    //parm_mip.bt_tech    = GLP_BT_BFS;       // Backtracking method: Breadth first seach
    //parm_mip.bt_tech    = GLP_BT_BLB;       // Backtracking method: Best Local Bound
    //parm_mip.bt_tech    = GLP_BT_BPH;       // Backtracking method: Best projection Heuristic


    glp_init_smcp(&parm_spx);
    /* Message level for the simplex solver */
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

    /*
     * Builds the matrix, row by row.
     * The graph adjacency matrix if flattened, get_index aids that.
     */
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

        //glp_simplex(lp, &parm_spx);
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
        sprintf(fname, "w_%s_%05d.svg", "", cycles);
        save_svg(fname, solution, n, nodes, scale);

        /*
         * Removes all the subtours
         */
        std::vector<std::vector<int> > all_cycles = find_all_cycles(solution, n);

        if ( all_cycles.size() == 1 ) {
            //printf("Done!!\n");
            break;
        }

        printf("\nStep: %d \n found %d cycles\n", cycles, (int)all_cycles.size());

        int ind   = 0;

        for ( unsigned int k = 0 ; k < all_cycles.size() ; k++ ){
            if (all_cycles[k].size() < all_cycles[ind].size()){
                ind = k;
            }
        }

        for ( unsigned int k = 0 ; k < all_cycles.size() ; k++ ){
            std::vector<int> small = all_cycles[k];

            printf(" -> removing cycle of size %d: ", (int)small.size() - 1);

            if ( small.size() > n/2 + 1 ) {
                printf(" Too big, skipping...\n");
                continue;
            }
            //if ( all_cycles[k].size() != all_cycles[ind].size() ) continue;

            for ( int i = 0 ; i < small.size() ; i++ ) {
                printf("%d ", small[i]+1);
            }
            printf("\n");

            int new_row = glp_add_rows(lp, 1);

            std::vector<int   > row;
            std::vector<double> val;

            row.push_back(0);
            val.push_back(0);

            // Standard cut: removed a cycle only;
            //for ( unsigned int i = 0 ; i < small.size()-1 ; i++ ) {
                //int index = get_index(small[i], small[i+1], n);

                ////printf("%d %d %d %02d %d\n", i, small[i], small[i+1], index, small.size());

                //row.push_back(index);
                //val.push_back(1    );
            //}

            /*
             * Decent cut: Actually does a cut and removes all possible cycles to that "island"
             * IT IS FAST!!!
             */
            for ( unsigned int i = 1 ; i < small.size()-1 ; i++ ) {
                for ( unsigned int j = 0 ; j < i ; j++ ) {
                    int index = get_index(small[j], small[i], n);

                    //printf("%d %d %d %02d %d\n", i, small[i], small[i+1], index, small.size());

                    row.push_back(index);
                    val.push_back(1    );
                }
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
    fprintf(stdout, "%d %f %.3f\n", n, (double)t/CLOCKS_PER_SEC, z);

    return EXIT_SUCCESS;

}
