#ifndef SVG_OUTPUT_H
#define SVG_OUTPUT_H

#include <vector>

void save_svg(char *fname, std::vector<std::vector<double> > &solution, int n, std::vector<node> nodes, int scale );

#endif /*  */
