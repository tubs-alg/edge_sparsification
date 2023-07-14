#include "rng/RelativeNeighborhoodGraph.h"

namespace Sparsification {

    RelativeNeighborhoodGraph::RelativeNeighborhoodGraph(EdgeSparsification &_instance) : instance(_instance) {
        // naive approach
        DelaunayTriangulation dt{instance.points.begin(), instance.points.end()};

        for (auto it = dt.finite_edges_begin(); it != dt.finite_edges_end(); ++it) {
            Segment_2 seg = dt.segment(it);
            bool empty = true;
            auto distance = seg.squared_length();
            for (auto &p : instance.points) {
                if (CGAL::squared_distance(seg.source(), p) < distance &&
                    CGAL::squared_distance(seg.target(), p) < distance) {
                    empty = false;
                    break;
                }
            }
            if (!empty) {
                continue;
            }
            instance.insertEdge(seg.source(), seg.target());
        }
    }

}
