#ifndef SPARSIFICATION_DEGREE_MST_SOLUTION_H
#define SPARSIFICATION_DEGREE_MST_SOLUTION_H

#include "solver/Solution.h"
#include "solver/Instance.h"
#include "definitions/global_definitions.h"
#include "utils/json_utils.hpp"
#include "utils/gnuplot_output_2.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace dmst {

    class Degree_mst_solution : public Solver::Solution {

    public:

        json json_dmst;
        Graph g;
        std::string solver_type;
        Solver::Instance instance;
        std::vector<Graph::edge_descriptor> solution_edges;

        Degree_mst_solution();

        Degree_mst_solution(Solver::Instance &_instance, const std::string& solver);

        void to_json(std::string fn) override;

        void put_json_size_t(const std::string& key, std::size_t value);

        void put_json_int(const std::string& key, int value);

        void put_json_double(const std::string& key, double value);

        void plot(std::string fn) override;

        void put_model_solution(json grbJson);

        void put_edges(const std::vector<Graph::edge_descriptor> &edges);

    };

}

#endif //SPARSIFICATION_DEGREE_MST_SOLUTION_H
