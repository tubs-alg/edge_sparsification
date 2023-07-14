#ifndef SPARSIFICATION_ALPHA_MST_SOLUTION_H
#define SPARSIFICATION_ALPHA_MST_SOLUTION_H

#include "solver/Solution.h"
#include "solver/Instance.h"
#include "definitions/global_definitions.h"
#include "utils/json_utils.hpp"
#include "utils/gnuplot_output_2.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace amst {

    class Alpha_mst_solution : public Solver::Solution {

    public:

        json json_amst;
        Graph g;
        std::string solver_type;
        Solver::Instance instance;
        std::vector<Graph::edge_descriptor> solution_edges;

        Alpha_mst_solution();

        Alpha_mst_solution(Solver::Instance &_instance, const std::string& solver);

        void to_json(std::string fn) override;

        void put_json_size_t(const std::string& key, std::size_t value);

        void put_json_int(const std::string& key, int value);

        void put_json_double(const std::string& key, double value);

        void plot(std::string fn) override;

        void put_model_solution(json grbJson);

        void put_edges(const std::vector<Graph::edge_descriptor> &edges);

        void put_cones(const std::vector<std::pair<std::size_t, std::size_t>> &edges);

    };

}

#endif //SPARSIFICATION_ALPHA_MST_SOLUTION_H
