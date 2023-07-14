#ifndef MINIMUM_WEIGHT_TRIANGULATION_ALGORITHMS_H
#define MINIMUM_WEIGHT_TRIANGULATION_ALGORITHMS_H

#include <iterator>
#include <algorithm>

#include <CGAL/Triangle_2.h>
#include <CGAL/Triangulation_2.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Kernel_traits.h>

#include "Mwt_traits.h"
#include "Minimum_weight_triangulation.h"

namespace CGAL {

    template<class InputIterator, class OutputIterator>
    OutputIterator diamond_filter(InputIterator first, InputIterator last, OutputIterator output) {

        (void) first;
        (void) last;

        return output;

    }


    // Caution: slow brute force function
    template<class InputIterator, class OutputIterator>
    OutputIterator naive_diamond_filter(InputIterator first, InputIterator last, OutputIterator output) {

        using Kernel = typename Kernel_traits<typename std::iterator_traits<InputIterator>::value_type>::Kernel;
        using Traits = typename Minimum_weight_triangulation::Mwt_traits_2<Kernel>;
        using Point_2 = typename Traits::Point_2;
        using Segment_2 = typename Traits::Segment_2;

        auto dt = Traits{}.diamond_test_2_object();

        for(auto it_p = first; it_p != last ; ++it_p) {
            for(auto it_q = std::next(it_p); it_q != last; ++it_q) {
                auto p = *it_p;
                auto q = *it_q;

                auto in_left_triangle = [&p,q,dt](const Point_2& x) { return x != q && x != p && dt(p, q, x); };
                auto in_right_triangle = [&p,q,dt](const Point_2& x) { return x != q && x != p && dt(q, p, x); };

                if(!(std::any_of(first, last, in_left_triangle) && std::any_of(first, last, in_right_triangle)))
                    output++ = Segment_2{p,q};

            }
        }

        return output;

    }


    template<class InputIterator>
    void construct_mwt_triangulation(InputIterator first, InputIterator last) {
        (void) first;
        (void) last;
    }

    template<class InputIterator, class OutputIterator>
    OutputIterator lmt_skeleton(InputIterator first, InputIterator last, OutputIterator out) {
        using Kernel = typename Kernel_traits<typename std::iterator_traits<InputIterator>::value_type>::Kernel;
        using Traits = typename Minimum_weight_triangulation::Mwt_traits_2<Kernel>;
        using Point_2 = typename Traits::Point_2;
        using Segment_2 = typename Traits::Segment_2;

        using Edge = std::pair<Point_2, Point_2>;

        std::vector<Edge> lmt_skeleton;

        CGAL::Minimum_weight_triangulation_2<Traits> mwt{first, last};
        mwt.output_lmt_skeleton(std::back_inserter(lmt_skeleton));

        for(auto&& edge : lmt_skeleton) {
            ++out = Segment_2{edge.first, edge.second};
        }

        return out;
    }

    template<class InputIterator, class OutputIterator>
    std::pair<OutputIterator, OutputIterator> lmt_skeleton(
            InputIterator first, InputIterator last, OutputIterator out_skeleton, OutputIterator out_candidates) {
        using Kernel = typename Kernel_traits<typename std::iterator_traits<InputIterator>::value_type>::Kernel;
        using Traits = typename Minimum_weight_triangulation::Mwt_traits_2<Kernel>;
        using Point_2 = typename Traits::Point_2;
        using Segment_2 = typename Traits::Segment_2;

        using Edge = std::pair<Point_2, Point_2>;

        std::vector<Edge> lmt_skeleton;
        std::vector<Edge> lmt_candidates;

        CGAL::Minimum_weight_triangulation_2<Traits> mwt{first, last};
        mwt.output_lmt_skeleton(std::back_inserter(lmt_skeleton));
        mwt.output_candidates(std::back_inserter(lmt_candidates));

        for(auto&& edge : lmt_skeleton) {
            ++out_skeleton = Segment_2{edge.first, edge.second};
        }


        for(auto&& edge : lmt_candidates) {
            ++out_candidates = Segment_2{edge.first, edge.second};
        }

        return std::make_pair(out_skeleton, out_candidates);
    }


    /**
     * The beta-skeleton is a subgraph of the MWT for beta > 1/sin(arctan(3/sqrt(2*sqrt(3))) = ~1.17682
     * (On beta-skeleton as a subgraph of the minimum weight triangulation by Cheng and Xu)
     * @param first
     * @param last
     * @param output
     * @return Edges of the beta-skeleton
     */
    template<class InputIterator, class OutputIterator>
    OutputIterator beta_skeleton(InputIterator first, InputIterator last, OutputIterator output) {

        using Kernel = typename Kernel_traits<typename std::iterator_traits<InputIterator>::value_type>::Kernel;
        using Traits = typename Minimum_weight_triangulation::Mwt_traits_2<Kernel>;
        //using Point_2 = typename Traits::Point_2;
        using Segment_2 = typename Traits::Segment_2;


        // The beta-skeleton is a subgraph of the Delaunay-Triangulation for beta > 1
        auto dt = Delaunay_triangulation_2<Traits>{first, last};
        if(dt.dimension() < 2) {
            return output; //TODO: Handle degenerate case
        }

        for (auto edge_it = dt.finite_edges_begin(); edge_it != dt.finite_edges_end(); ++edge_it) {

            Segment_2 seg = dt.segment(edge_it);
            //const Point_2& p = seg.source();
            //const Point_2& q = seg.target();

            auto face = edge_it->first;
            auto index = edge_it->second;

            auto vertex = face->vertex(index);
            auto mirror_vertex = dt.mirror_vertex(face, index);

            if(dt.is_infinite(vertex) || dt.is_infinite(mirror_vertex)) {
                //Edge is on the convex hull
                //TODO: Decide how to handle convex hull edges, not all belong to the skeleton, but all are part of MWT
                output++ = seg;
                continue;
            }

            auto beta_test = typename Traits::Is_in_beta_skeleton_2{};

            if(beta_test(seg, vertex->point(), mirror_vertex->point()))
                output++ = seg;

        }

        return output;
    }

    template <typename Triangulation>
    double compute_squared_edge_total_weight(Triangulation tr) {
        double sum{0};
        for (auto edge_it = tr.finite_edges_begin(); edge_it != tr.finite_edges_end(); ++edge_it) {
            auto seg = tr.segment(edge_it);
            sum += seg.squared_length();
        }

        return sum;
    }

}

#endif //MINIMUM_WEIGHT_TRIANGULATION_ALGORITHMS_H
