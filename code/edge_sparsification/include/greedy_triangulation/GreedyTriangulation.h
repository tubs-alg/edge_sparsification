#ifndef SPARSIFICATION_GREEDYTRIANGULATION_H
#define SPARSIFICATION_GREEDYTRIANGULATION_H

#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_with_id_2.h>
#include <CGAL/boost/graph/graph_traits_Constrained_Delaunay_triangulation_2.h>
#include "definitions/global_definitions.h"
#include "edge_sparsification/EdgeSparsification.h"

namespace Sparsification {

    class GreedyTriangulation {

        using vb = CGAL::Triangulation_vertex_base_with_id_2<Kernel>;
        using fb = CGAL::Constrained_triangulation_face_base_2<Kernel>;
        using TDS = CGAL::Triangulation_data_structure_2<vb, fb>;
        using ConstrainedDelaunay = CGAL::Constrained_Delaunay_triangulation_2<Kernel, TDS>;
        using Segment = ConstrainedDelaunay::Segment;
        using Edge = ConstrainedDelaunay::Edge;
        using Edge_circulator = TDS::Edge_circulator;

    private:
        EdgeSparsification & instance;
        ConstrainedDelaunay cdt;

    public:

        static bool sort_pair_double_segment(const std::pair<double, Segment> &a, const std::pair<double, Segment> &b);

        static bool sort_pair_double_segment_desc(const std::pair<double, Segment> &a, const std::pair<double, Segment> &b);

        explicit GreedyTriangulation(EdgeSparsification & _instance, bool min = true);

        void triangulate_naive(bool min = true);

        void triangulate_goldman();

    };

}

#endif //SPARSIFICATION_GREEDYTRIANGULATION_H
