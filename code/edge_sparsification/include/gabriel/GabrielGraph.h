#ifndef SPARSIFICATION_GABRIELGRAPH_H
#define SPARSIFICATION_GABRIELGRAPH_H

#include <vector>

#include "definitions/global_definitions.h"
#include "edge_sparsification/EdgeSparsification.h"
#include <CGAL/Delaunay_triangulation_2.h>

using DelaunayTriangulation = CGAL::Delaunay_triangulation_2<Kernel>;
using Circle = Kernel::Circle_2;

namespace Sparsification {

    class GabrielGraph {

    public:
        GabrielGraph(EdgeSparsification & _instance);

    private:

        EdgeSparsification & instance;

    };

}

#endif //SPARSIFICATION_GABRIELGRAPH_H

