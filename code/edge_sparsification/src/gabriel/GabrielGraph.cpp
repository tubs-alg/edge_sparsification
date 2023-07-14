#include "gabriel/GabrielGraph.h"

namespace Sparsification {

    GabrielGraph::GabrielGraph(EdgeSparsification &_instance) : instance(_instance) {
        // naive approach
        DelaunayTriangulation dt{instance.points.begin(), instance.points.end()};

        for (auto it = dt.finite_edges_begin(); it != dt.finite_edges_end(); ++it) {
            Segment_2 seg = dt.segment(it);
            Circle circ{seg.source(), seg.target()};
            bool empty = true;
            for (auto & p : instance.points) {
                if (p == seg.source() || p == seg.target()) {
                    continue;
                }
                if (circ.has_on_bounded_side(p) || circ.has_on_boundary(p)) {
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

