#include <vector>
#include <cstdio>

#include "types.h"
#include "svg_output.h"

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
    fprintf(fptr,"<svg width=\"%d%%\" height=\"%d%%\" version=\"1.0\" \n", screenx, screeny);
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
