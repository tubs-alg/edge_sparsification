#ifndef SPARSIFICATION_GREEDYSPANNER_H
#define SPARSIFICATION_GREEDYSPANNER_H

#include "definitions/global_definitions.h"
#include "edge_sparsification/EdgeSparsification.h"
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/graph_utility.hpp>

using idx_pair = std::pair<std::size_t, std::size_t>;

namespace Sparsification {

    bool sort_pair_double_idx_pair(const std::pair<double, idx_pair> &a, const std::pair<double, idx_pair> &b);

    class GreedySpanner {

    public:

        GreedySpanner(EdgeSparsification & _instance, double _t);

        void compute_spanner();

    private:
        EdgeSparsification & instance;
        double t;
        std::vector<std::pair<double, idx_pair>> weightedEdgeList;

        void init_weights();

    };

}

#endif //SPARSIFICATION_GREEDYSPANNER_H