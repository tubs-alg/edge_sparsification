#include "greedy_triangulation/GreedyTriangulation.h"

namespace Sparsification {

    bool GreedyTriangulation::sort_pair_double_segment(const std::pair<double, Segment> &a, const std::pair<double, Segment> &b) {
        return a.first < b.first;
    }

    bool GreedyTriangulation::sort_pair_double_segment_desc(const std::pair<double, Segment> &a, const std::pair<double, Segment> &b) {
        return a.first > b.first;
    }

    GreedyTriangulation::GreedyTriangulation(EdgeSparsification & _instance, bool min) : instance(_instance)  {
        if (min) {
            triangulate_goldman();
        } else {
            triangulate_naive(min);
        }
    }

    void GreedyTriangulation::triangulate_naive(bool min) {
        std::vector<std::pair<double, Segment>> distance_vector;

        typename boost::graph_traits<Graph>::vertex_iterator s, t, end;
        for (boost::tie(s, end) = boost::vertices(instance.g); s != end; ++s) {
            for (t = s+1; t != end; ++t) {
                double distance = CGAL::sqrt(CGAL::squared_distance(instance.points[*s], instance.points[*t]));
                distance_vector.emplace_back(std::make_pair(distance, Segment{instance.points[*s], instance.points[*t]}));
            }
        }

        if (min) {
            std::sort(distance_vector.begin(), distance_vector.end(), sort_pair_double_segment);
        } else {
            std::sort(distance_vector.begin(), distance_vector.end(), sort_pair_double_segment_desc);
        }

        bool incomplete = true;
        std::vector<Segment> segments;

        for (int i = 0; i < distance_vector.size(); ++i) {
            bool intersects = false;
            for (auto seg : segments) {
                if (CGAL::do_intersect(distance_vector[i].second, seg)) {
                    if (distance_vector[i].second.source() == seg.source() || distance_vector[i].second.source() == seg.target()
                    || distance_vector[i].second.target() == seg.source() || distance_vector[i].second.target() == seg.target()) {
                        continue;
                    }
                    intersects = true;
                    break;
                }
            }
            if (intersects) {
                continue;
            } else {
                segments.emplace_back(distance_vector[i].second);
                instance.insertEdge(instance.point_map[distance_vector[i].second.source()], instance.point_map[distance_vector[i].second.target()]);
            }
        }
    }

    void GreedyTriangulation::triangulate_goldman() {

        cdt.insert(instance.points.begin(), instance.points.end());

        bool incomplete = true;

        int cnt = 0;
        while (incomplete) {
            cnt++;
            incomplete = false;
            double candidate_length = std::numeric_limits<double>::infinity();
            Segment candidate_edge;
            // shortest edge of constrained delauny greedy_triangulation that is not yet contained in g
            for (auto & e : cdt.finite_edges()) {
                // iterate over all cdt edges
                if (!cdt.is_constrained(e)) {
                    // edge is not constrained -> greedy triangulation is incomplete
                    incomplete = true;
                    auto len = std::sqrt(cdt.segment(e).squared_length());
                    if (len < candidate_length) {
                        candidate_length = len;
                        candidate_edge = cdt.segment(e);
                    }
                }
            }

            /*
             * Invariant for finding the next edge tbi with use of Edge_circulator
             */
            for (auto & v : cdt.finite_vertex_handles()) {
                std::vector<Edge> edges;
                Edge_circulator ec = cdt.incident_edges(v), done(ec);
                do {
                    if (cdt.is_constrained(*ec)) {
                        edges.push_back(*ec);
                    }
                } while (++ec != done);
                if (edges.size() > 1) {
                    // if there are at least two constrained incident edges
                    // take every two ccw consecutive edges and check the edge that connects both ends
                    for (std::size_t i = 0; i < edges.size(); ++i) {
                        std::size_t j = (i+1) % edges.size();

                        Point_2 q = cdt.segment(edges[i]).source();
                        Point_2 r = cdt.segment(edges[j]).source();
                        Segment seg{q, r};
                        auto s = instance.point_map[q];
                        auto t = instance.point_map[r];
                        if (!boost::edge(s, t, instance.g).second) {
                            // edge not already existing
                            auto orientation = CGAL::orientation(v->point(), q, r);
                            if (orientation == CGAL::COLLINEAR) continue;
                            if (orientation == CGAL::RIGHT_TURN && edges.size() > 2) {
                                Point_2 z = edges[(i-1) % edges.size()].first->vertex((edges[(i-1) % edges.size()].second+1)%3)->point();
                                if (CGAL::do_intersect(seg, Ray_2(v->point(), z))) {
                                    continue;
                                }
                            }
                            auto length = CGAL::sqrt(seg.squared_length());
                            if (length < candidate_length) {
                                candidate_length = length;
                                candidate_edge = seg;
                            }
                        }
                    }
                }
            }

            if (incomplete) {
                auto si = instance.point_map[candidate_edge.source()];
                auto ti = instance.point_map[candidate_edge.target()];
                instance.insertEdge(si, ti);
                cdt.insert_constraint(candidate_edge.source(), candidate_edge.target());
            }
        }
    }

}