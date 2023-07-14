#ifndef SPARSIFICATION_RELATIVENEIGHBORHOODGRAPH_H
#define SPARSIFICATION_RELATIVENEIGHBORHOODGRAPH_H

#include <vector>

#include "definitions/global_definitions.h"
#include "edge_sparsification/EdgeSparsification.h"
#include <CGAL/Delaunay_triangulation_2.h>

using DelaunayTriangulation = CGAL::Delaunay_triangulation_2<Kernel>;


namespace Sparsification {

    class RelativeNeighborhoodGraph {

    public:
        RelativeNeighborhoodGraph(EdgeSparsification & _instance);

    private:

        EdgeSparsification & instance;

    };

}


#endif //SPARSIFICATION_RELATIVENEIGHBORHOODGRAPH_H