#include "greedy_spanner/GreedySpanner.h"

namespace Sparsification {

    bool sort_pair_double_idx_pair(const std::pair<double, idx_pair> &a, const std::pair<double, idx_pair> &b) {
        return a.first < b.first;
    }

    GreedySpanner::GreedySpanner(EdgeSparsification & _instance, double _t) : instance(_instance) {
        t = _t;
        init_weights();
        compute_spanner();
    }

    void GreedySpanner::init_weights() {
        for (std::size_t s_i = 0; s_i < instance.points.size()-1; ++s_i) {
            for (std::size_t t_i = s_i+1; t_i < instance.points.size(); ++t_i) {
                double distance = CGAL::sqrt(CGAL::squared_distance(instance.points[s_i], instance.points[t_i]));
                weightedEdgeList.emplace_back(distance, std::make_pair(s_i, t_i));
            }
        }
        std::sort(weightedEdgeList.begin(), weightedEdgeList.end(), sort_pair_double_idx_pair);
    }

    void GreedySpanner::compute_spanner() {
        for (auto const &[weight, edge] : weightedEdgeList) {
            std::vector<vertex_descriptor> p(num_vertices(instance.g));
            std::vector<double> d(num_vertices(instance.g));
            vertex_descriptor v = boost::vertex(edge.first, instance.g);
            boost::dijkstra_shortest_paths(instance.g, v,
                                           predecessor_map(
                                                   make_iterator_property_map(p.begin(), get(boost::vertex_index, instance.g))).
                                                   distance_map(
                                                   make_iterator_property_map(d.begin(), get(boost::vertex_index, instance.g)))
                                           );
            if ((d[edge.second] / weight) > t) {
                instance.insertEdge(edge.first, edge.second);
            }
        }
    }

}